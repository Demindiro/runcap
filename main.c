#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <linux/securebits.h>
#include <linux/capability.h>
#include <sys/capability.h>

int main(int argc, char **argv) {

	// Check arguments
	if (argc <= 3) {
		fprintf(
			stderr,
			"Usage: %s <user> <capabilities> <program> [args...]\n",
			argc > 0 ? argv[0] : "runcap"
		);
		return 1;
	}

	// Get UID & GID
	struct passwd *pwd = getpwnam(argv[1]);
	if (pwd == NULL) {
		if (errno == 0) {
			fprintf(stderr, "User not found\n");
		} else {
			perror("getpwnam()");
		}
		return 1;
	}

	// Set UID & GID
	// We're still privileged after this.
	if (cap_setgroups(pwd->pw_gid, 1, &pwd->pw_gid) < 0) {
		perror("cap_setgroups()");
		return 1;
	}
	if (cap_setuid(pwd->pw_uid) < 0) {
		perror("cap_setuid()");
		return 1;
	}

	// Parse capabilities
	// Append eip as the capabilities need to be in all those sets.
	size_t n = strlen(argv[2]);
	char *caps_str = malloc(n + 4 + 1);
	memcpy(caps_str, argv[2], n);
	memcpy(caps_str + n, "+eip", 5);
	puts(caps_str);

	cap_t caps = cap_from_text(caps_str);
	if (caps == NULL) {
		perror("cap_from_text()");
		return 1;
	}

	// Set capabilities
	if (cap_set_proc(caps) < 0) {
		perror("cap_set_proc()");
		return 1;
	}

	// Keep capabilities on exec()
	char *name = strtok(argv[2], ",");
	while (name != NULL) {
		cap_value_t cap;
		if (cap_from_name(name, &cap) < 0) {
			perror("cap_from_name()");
			return 1;
		}
		if (cap_set_ambient(cap, CAP_SET) < 0) {
			perror("cap_set_ambient()");
			return 1;
		}
		name = strtok(NULL, ",");
	}

	// Execute program
	execvp(argv[3], &argv[3]);
	perror("execvp()");
	return 1;
}
