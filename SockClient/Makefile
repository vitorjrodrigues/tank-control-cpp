BIN = Client

Client: Client.o create_socket_inet.o
	cc -c -o create_socket_inet.o create_socket_inet.cpp
	cc -c -o Client.o Client.cpp
	g++ -o Client Client.o create_socket_inet.o -lpthread -lm

clean:
	rm -f *.o $(BIN)