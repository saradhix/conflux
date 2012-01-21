flux:	flux.o connection.o memory.o
	gcc flux.o connection.o memory.o -o flux

flux.o:	flux.c
	gcc -c flux.c
connection.o:	connection.c
	gcc -c connection.c 

memory.o:	memory.c
	gcc -c memory.c

client:	client.c
	gcc client.c -o client

