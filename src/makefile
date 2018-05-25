FLAGS=-g -Wall

all: ui

ui: simple_socket.o ui.o
	g++ -o ui ui.o simple_socket.o

ui.o: connection.h simple_socket.h ui.cpp
	g++ $(FLAGS) -c ui.cpp
	
simple_socket.o: simple_socket.h simple_socket.cpp
	g++ $(FLAGS) -c simple_socket.cpp
	
clean:
	rm -rf *.o ui