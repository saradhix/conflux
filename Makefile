PROGRAMS := flux client skeleton1 skeleton2
UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
#specific to linux
endif
ifeq ($(UNAME), Solaris)
#specific to solaris - include lsocket and lnsl
LIBS = -lsocket -lnsl
endif
all:	${PROGRAMS}
flux:	flux.o connection.o memory.o log.o messages.o net.o
	gcc flux.o connection.o memory.o log.o messages.o net.o -o flux ${LIBS}
flux.o:	flux.c
	gcc -c flux.c
connection.o:	connection.c
	gcc -c connection.c 
memory.o:	memory.c
	gcc -c memory.c
log.o:	log.c
	gcc -c log.c
messages.o:	messages.c
	gcc -c messages.c
client:	client.c net.c log.c
	gcc client.c net.c log.c -o client ${LIBS}
net.o:	net.c
	gcc -c net.c
skeleton1:	skeleton1.c net.c log.c
	gcc skeleton1.c net.c log.c -o skeleton1 ${LIBS}
skeleton2:	skeleton2.c net.c log.c
	gcc skeleton2.c net.c log.c -o skeleton2 ${LIBS}
clean:
	rm -f ${PROGRAMS} *.o
