BIN=Client

CFLAGS += -std=c99

all: $(BIN)

Client: Client.o create_socket_inet.o
	gcc -o $@ Client.o create_socket_inet.o

clean:
	rm -f *.o $(BIN)