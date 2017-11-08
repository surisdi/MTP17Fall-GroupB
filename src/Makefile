


default: main

#main: Compressor.o Protocol.o Socket.o utils.o main.cpp Compressor.hpp Protocol.hpp Socket.hpp utils.hpp 
#	g++ -L home/pi/RF24/ -lrf24 -o ../main main.cpp Compressor.o Protocol.o Socket.o utils.o -fno-omit-frame-pointer -O2 -pthread

main: Compressor.o Protocol.o Socket.o utils.o main.cpp Compressor.hpp Protocol.hpp Socket.hpp utils.hpp 
	g++ -std=gnu++11 -o ../main main.cpp Compressor.o Protocol.o Socket.o utils.o -fno-omit-frame-pointer -O2 -pthread

Protocol.o: Protocol.cpp Protocol.hpp
	g++ -std=gnu++11 -c Protocol.cpp

Compressor.o: Compressor.cpp Compressor.hpp
	g++ -std=gnu++11 -c Compressor.cpp -fno-omit-frame-pointer -O2

Socket.o: Socket.cpp Socket.hpp
	g++ -std=gnu++11 -L home/pi/RF24/ -lrf24 -c Socket.cpp -fno-omit-frame-pointer -O2


utils.o: utils.cpp utils.hpp
	g++ -std=gnu++11 -c utils.cpp -fno-omit-frame-pointer -O2

clean:
	rm -r *.o
