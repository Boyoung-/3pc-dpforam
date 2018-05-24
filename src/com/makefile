FLAGS=-g -Wall

all: server client

client: simple_socket.o client.o
	g++ -o client client.o simple_socket.o

client.o: connection.h simple_socket.h client.cpp
	g++ $(FLAGS) -c client.cpp
	
server: simple_socket.o server.o
	g++ -o server server.o simple_socket.o
	
server.o: connection.h simple_socket.h server.cpp
	g++ $(FLAGS) -c server.cpp
	
simple_socket.o: simple_socket.h simple_socket.cpp
	g++ $(FLAGS) -c simple_socket.cpp
	
clean:
	rm -rf *.o server client