all: client server

client: client.c
	gcc client.c -std=c99 -Wall -o client

server: server.c
	gcc server.c -std=c99 -lpthread -Wall -o server

remove: 
	rm -f server client