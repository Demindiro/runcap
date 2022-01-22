CC ?= cc
DESTDIR ?= /usr/local/bin

runcap: main.c
	$(CC) -Os $< -o $@ -lcap

install:
	cp runcap $(DESTDIR)

clean:
	rm -f runcap
