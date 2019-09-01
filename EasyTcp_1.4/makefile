GCC=g++
CFLAGS=-Wall -g -std=c++11

all:Server Client

Server:Server.cpp pub.h
	$(GCC) -o Server Server.cpp $(CFLAGS) -pthread
Client:Client.cpp pub.h
	$(GCC)  -o Client Client.cpp $(CFLAGS) -pthread

clean:
	rm Server Client *.o -rf
