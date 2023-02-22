all:
	cc -O3 -o dgraph dgraph.c -lm -lncurses
install:
	install ./dgraph /usr/bin/dgraph
clean:
	rm dgraph
