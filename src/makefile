FLAGS=-g -Wall
LIBPATH=-L"../lib"
LIBS=-lcryptopp -lcrypto

all: ui test

ui: connection.o inslbl.o protocol.o simple_socket.o ssot.o ui.o util.o
	g++ -o ui connection.o inslbl.o protocol.o simple_socket.o ssot.o ui.o util.o $(LIBPATH) $(LIBS)
	
connection.o: connection.h util.h connection.cpp
	g++ $(FLAGS) -c connection.cpp
	
fss.o: fss.h util.h fss.cpp
	g++ $(FLAGS) -c fss.cpp
	
inslbl.o: inslbl.h util.h inslbl.cpp
	g++ $(FLAGS) -c inslbl.cpp
	
prg256.o: prg256.h prg256.cpp
	g++ $(FLAGS) -c prg256.cpp
	
protocol.o: protocol.h protocol.cpp
	g++ $(FLAGS) -c protocol.cpp
	
simple_socket.o: simple_socket.h simple_socket.cpp
	g++ $(FLAGS) -c simple_socket.cpp
	
ssot.o: ssot.h util.h ssot.cpp
	g++ $(FLAGS) -c ssot.cpp

ui.o: connection.h inslbl.h protocol.h simple_socket.h ssot.h ui.cpp
	g++ $(FLAGS) -c ui.cpp
	
util.o: util.h util.cpp
	g++ $(FLAGS) -c util.cpp
	
clean:
	rm -rf *.o ui test
	
test: fss.o prg256.o test.o util.o
	g++ -o test fss.o prg256.o test.o util.o $(LIBPATH) $(LIBS)
	
test.o: fss.h test.cpp
	g++ $(FLAGS) -c test.cpp