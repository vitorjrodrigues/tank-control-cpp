BIN=JSReader

CFLAGS += -std=c99

all: $(BIN)

JSReader: JSReader.o create_socket_inet.o
	gcc -o $@ JSReader.o create_socket_inet.o

clean:
	rm -f *.o $(BIN)
	
