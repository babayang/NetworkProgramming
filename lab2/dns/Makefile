all:server client
server:server.o 
	gcc  -w -o server server.o  
client:client.o
	gcc -w -o client client.o
server.o:server.c dns_protocol.h
	gcc -w -c server.c
client.o:client.c dns_protocol.h
	gcc -w -c client.c
clean:
	rm -f *.o
