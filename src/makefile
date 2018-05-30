FLAGS=-g -Wall
LIBPATH=-L"../lib"
LIBS=-lcryptopp

all: ui

ui: ui.o simple_socket.o ssot.o util.o
	g++ -o ui ui.o simple_socket.o util.o ssot.o $(LIBPATH) $(LIBS)

ui.o: connection.h simple_socket.h ssot.h ui.cpp
	g++ $(FLAGS) -c ui.cpp
	
ssot.o: ssot.h util.h ssot.cpp
	g++ $(FLAGS) -c ssot.cpp
	
simple_socket.o: simple_socket.h simple_socket.cpp
	g++ $(FLAGS) -c simple_socket.cpp
	
util.o: util.h util.cpp
	g++ $(FLAGS) -c util.cpp
	
clean:
	rm -rf *.o ui