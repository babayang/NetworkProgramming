all:server client
server:server.c
	gcc -w server.c -o server -pthread 
client:client.c
	gcc -w client.c -o client -pthread

clean:
	rm -f client server
