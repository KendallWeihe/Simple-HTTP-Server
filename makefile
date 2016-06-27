default: http_server

http_server: HTTPTCPServer-Fork.o AcceptTCPConnection.o DieWithError.o CreateTCPServerSocket.o HTTPHandleTCPClient.o
	gcc -g -Wall HTTPTCPServer-Fork.o AcceptTCPConnection.o DieWithError.o CreateTCPServerSocket.o HTTPHandleTCPClient.o -o http_server

HTTPTCPServer-Fork.o: HTTPTCPServer-Fork.c
	gcc -c HTTPTCPServer-Fork.c

AcceptTCPConnection.o: AcceptTCPConnection.c
	gcc -c AcceptTCPConnection.c

DieWithError.o: DieWithError.c
	gcc -c DieWithError.c

CreateTCPServerSocket.o: CreateTCPServerSocket.c
	gcc -c CreateTCPServerSocket.c

HTTPHandleTCPClient.o: HTTPHandleTCPClient.c
	gcc -c HTTPHandleTCPClient.c

clean:
	rm http_server *.o
