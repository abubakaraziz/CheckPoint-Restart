# You may decide to replace 'gcc' by 'clang' if you have 'clang on your
# local machine.  It has better error reporting.

# Some reasonable tests to try this out might be:
# make all
# ./test1 primtes-test 12345678912345
# ./test1 counting-test 12345678912345

CC=gcc
CFLAGS=-g3 -O0

all: restart hello primes-test counting-test test1 test2 test3 test4 test5 \
     proc-self-maps save-restore-memory
#========================

primes-test: primes-test.c
	${CC} ${CFLAGS} -rdynamic -o $@ $<

counting-test: counting-test.c 
	${CC} ${CFLAGS} -rdynamic -o $@ $<
hello: hello.c
	${CC} ${CFLAGS} -rdynamic -o $@ $<

# https://stackoverflow.com/questions/36692315/what-exactly-does-rdynamic-do-and-when-exactly-is-it-needed
test: test.c
	${CC} ${CFLAGS} -o $@ $<

test%: test libconstructor%.so
	cp $< $@

# -fPIC required for shared libraries (.so)
libconstructor%.so: constructor%.o
	${CC} ${CFLAGS} -shared -fPIC -o $@ $<
constructor%.o: constructor%.c
	${CC} ${CFLAGS} -fPIC -c $<
#==========================
#My test make file
checkpoint: checkpoint.c libcpkt.so 
	${CC} ${CFLAGS} -o checkpoint checkpoint.c 

libcpkt.so: cpkt.o
	    ${CC} ${CFLAGS} -shared -fPIC -o libcpkt.so cpkt.o
 
cpkt.o: cpkt.c
	${CC} ${CFLAGS} -fPIC -c cpkt.c
#========================
proc-self-maps: proc-self-maps.c
	${CC} ${CFLAGS} -DSTANDALONE -o $@ $<

#========================
save-restore-memory: save-restore-memory.c
	${CC} ${CFLAGS} -o $@ $<

#========================
restart : restart.c
	gcc -static -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o restart restart.c 


#=======================
clean:
	rm -f a.out primes-test counting-test
	rm -f libconstructor?.so constructor?.o test? test
	rm -f proc-self-maps save-restore-memory save-restore.dat
	rm -f cpkt.o checkpoint libcpkt.so
dist: clean
	dir=`basename $$PWD` && cd .. && tar czvf $$dir.tar.gz ./$$dir
	dir=`basename $$PWD` && ls -l ../$$dir.tar.gz

.PRECIOUS: libconstructor%.so constructor%.o
