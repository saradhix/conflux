all:	flux client skeleton1 skeleton2
flux:	flux.o connection.o memory.o log.o messages.o net.o
	gcc flux.o connection.o memory.o log.o messages.o net.o -o flux
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
client:	client.c net.c
	gcc client.c net.c -o client
net.o:	net.c
	gcc -c net.c
skeleton1:	skeleton1.c net.c
	gcc skeleton1.c net.c -o skeleton1
skeleton2:	skeleton2.c net.c
	gcc skeleton2.c net.c -o skeleton2
