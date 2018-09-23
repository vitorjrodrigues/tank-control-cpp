BIN=Client

all: $(BIN)

Client: Client.o create_socket_inet.o
	cc -c -o Client.o Client.cpp
	cc -c -o create_socket_inet.o create_socket_inet.c
	g++ -o $@ Client.o create_socket_inet.o

clean:
	rm -f *.o $(BIN)