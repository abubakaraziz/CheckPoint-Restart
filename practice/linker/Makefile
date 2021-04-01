NY: all clean test

LDEXTRAFLAGS ?=

all: prog

bar.o: bar.c
	    gcc -c -Wall -fpic -o $@ $<

libbar.so: bar.o
	    gcc -shared -o $@ $<

main.o: main.c
	    gcc -c -Wall -o $@ $<

prog: main.o | libbar.so
	    gcc $(LDEXTRAFLAGS) -o $@ $< -L. -lbar -ldl

clean:
	    rm -f *.o *.so prog

test: prog
	    ./$<

