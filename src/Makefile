CC=g++
CFLAGS=-fno-omit-frame-pointer -O2 -pthread -lz -std=gnu++11
default: main

main: Compressor.o Protocol.o Protocol_StopWait.o Protocol_GoBackN.o Socket.o utils.o main.cpp Compressor.hpp Protocol.hpp Socket.hpp utils.hpp 
	g++ -o ../main main.cpp Compressor.o Protocol.o Protocol_GoBackN.o Protocol_StopWait.o Socket.o utils.o $(CFLAGS) -L home/pi/RF24/ -lrf24

#main: Compressor.o Protocol.o Protocol_StopWait.o Protocol_GoBackN.o Socket.o utils.o main.cpp Compressor.hpp Protocol.hpp Socket.hpp utils.hpp 
#	g++ -g -std=gnu++11 -o ../main main.cpp Compressor.o Protocol.o Protocol_GoBackN.o Protocol_StopWait.o Socket.o utils.o -fno-omit-frame-pointer -O2 -pthread -lz

Protocol.o: Protocol.cpp Protocol.hpp
	$(CC) -c Protocol.cpp $(CFLAGS)

Protocol_GoBackN.o: Protocol_GoBackN.cpp Protocol.hpp
	$(CC) -c Protocol_GoBackN.cpp $(CFLAGS)

Protocol_StopWait.o: Protocol_StopWait.cpp Protocol.hpp
	$(CC) -c Protocol_StopWait.cpp $(CFLAGS)

Compressor.o: Compressor.cpp Compressor.hpp
	$(CC) -c Compressor.cpp $(CFLAGS)

Socket.o: Socket.cpp Socket.hpp
	$(CC) -c Socket.cpp $(CFLAGS)

utils.o: utils.cpp utils.hpp
	$(CC) -c utils.cpp $(CFLAGS)

clean:
	rm -r *.o
	rm ../main
