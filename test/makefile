FLAGS=-g -Wall
LIBPATH=-L"../lib"
LIBS=-lcryptopp

all: test

test: test.o
	g++ -o test test.o $(LIBPATH) $(LIBS) 

test.o: test.cpp
	g++ $(FLAGS) -c test.cpp
	
clean:
	rm -rf *.o test