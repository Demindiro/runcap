# runcap

This program allows granting specific capabilities to non-root processes.

## Why not capsh?

`capsh` is (IMO) awful to use and I couldn't find an alternative. For some
reason the Debian version misses `--addamb` too which makes it effectively
useless. I also can't figure out how to make it `exec()` ("chainload") a
program.

## How it works

- Set UID & GID (with `set_uid` and `set_groups`, which are special).
- Put a list of capabilities in the effective, inheritable and permitted
  sets.
- Put those capabilities in the ambient set.
- Execute the program.

## TODO

It may be worth setting the bounding set properly.
