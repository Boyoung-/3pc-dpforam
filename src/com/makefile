FLAGS=-g -Wall

simple_socket: simple_socket.o
	g++ -o simple_socket simple_socket.o
	
simple_socket.o: connection.h simple_socket.h simple_socket.cpp
	g++ $(FLAGS) -c simple_socket.cpp
	
clean:
	rm -rf *.o simple_socket