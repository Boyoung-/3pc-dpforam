all: hello_world

hello_world: hello_world.o
	g++ -o hello_world hello_world.o

hello_world.o: hello_world.cpp
	g++ -c hello_world.cpp
	
server: server.o
	g++ -o server server.o
	
server.o: server.cpp
	g++ -c server.cpp
	
client: client.o
	g++ -o client client.o
	
client.o: client.cpp
	g++ -c client.cpp

clean:
	rm -f *.o hello_world