flux:	flux.o connection.o memory.o log.o messages.o
	gcc flux.o connection.o memory.o log.o messages.o -o flux
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

