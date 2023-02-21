all:
	if [! command -v cc &> /dev/null]
	then
		echo "No C compiler found. Please install one."
		exit 1
	fi
	if [! command -v gzexe &> /dev/null]
	then
		echo "gzexe could not be found"
		exit 1
	fi
	cc dgraph.c -O3 -lncurses -lm -o dgraph
	gzexe dgraph
	rm dgraph~

clean:
	rm dgraph
	rm dgraph~
install:
	cp dgraph /usr/bin
check:
	echo "No test suite is available for this program."
