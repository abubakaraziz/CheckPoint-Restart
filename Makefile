CC=gcc
CFLAGS=-g3 -O0 -fno-stack-protector

all: restart hello checkpoint
#========================

check: checkpoint restart hello
	rm -f cpkt.dat  # Remove any older 'myckpt' files
	./checkpoint ./hello &  # Start the target program in backgroun
	sleep 2
	echo Checkpointing ...
	kill -12 `pgrep -n hello`  # Send a SIGUSR2 to the pid of 'hello'
	sleep 2
	echo Restarting ...
	pkill -9 hello  # Kill your 'hello' program
	./restart cpkt	

#My test make file
checkpoint: checkpoint.c libcpkt.so 
	${CC} ${CFLAGS} -o checkpoint checkpoint.c 

libcpkt.so: cpkt.o
	    ${CC} ${CFLAGS} -shared -fPIC -o libcpkt.so cpkt.o
hello:	hello.c  
	${CC} ${CFLAGS} -o $@ $< 
cpkt.o: cpkt.c
	${CC} ${CFLAGS} -fPIC -c cpkt.c

#========================
restart : restart.c
	gcc -g -static -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o restart restart.c 


#=======================
clean:
	rm -f cpkt.o checkpoint libcpkt.so hello.o hello restart ckpt.dat
dist: clean
	dir=`basename $$PWD` && cd .. && tar czvf $$dir.tar.gz ./$$dir
	dir=`basename $$PWD` && ls -l ../$$dir.tar.gz

