FLAGS=-g -Wall
LIBPATH=-L"../lib"
LIBS=-lcryptopp

all: ui

ui: protocol.o simple_socket.o ssot.o ui.o util.o
	g++ -o ui protocol.o simple_socket.o ssot.o ui.o util.o $(LIBPATH) $(LIBS)
	
protocol.o: protocol.h protocol.cpp
	g++ $(FLAGS) -c protocol.cpp
	
simple_socket.o: simple_socket.h util.h simple_socket.cpp
	g++ $(FLAGS) -c simple_socket.cpp
	
ssot.o: ssot.h util.h ssot.cpp
	g++ $(FLAGS) -c ssot.cpp

ui.o: connection.h simple_socket.h ssot.h ui.cpp
	g++ $(FLAGS) -c ui.cpp
	
util.o: util.h util.cpp
	g++ $(FLAGS) -c util.cpp
	
clean:
	rm -rf *.o ui