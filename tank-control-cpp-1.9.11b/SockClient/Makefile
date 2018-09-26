all: Client

Client: Client.o Setup.o speedFeedback.o create_socket_inet.o
	g++ -o Client *.o -lpthread -lm

Client.o: Client.cpp create_socket.h
	g++ -o Client.o -c Client.cpp
	
Setup.o: Setup.cpp Setup.h
	g++ -o Setup.o -c Setup.cpp
	
speedFeedback.o: speedFeedback.cpp speedFeedback.h
	g++ -o speedFeedback.o -c speedFeedback.cpp

create_socket_inet.o: create_socket_inet.cpp create_socket.h
	c++ -o create_socket_inet.o -c create_socket_inet.cpp
	
clean:
	rm -f *.o Client
