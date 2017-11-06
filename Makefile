


default: main

main: Compressor.o Protocol.o Socket.o utils.o main.cpp Compressor.hpp Protocol.hpp Socket.hpp utils.hpp 
	g++ -L home/pi/RF24/ -lrf24 -o ../main main.cpp Compressor.o Protocol.o Socket.o utils.o -fno-omit-frame-pointer -O2


Compressor.o: Compressor.cpp Compressor.hpp
	g++ -c Compressor.cpp -fno-omit-frame-pointer -O2

Socket.o: Socket.cpp Socket.hpp
	g++ -L home/pi/RF24/ -lrf24 -c Socket.cpp -fno-omit-frame-pointer -O2


utils.o: utils.cpp utils.hpp
	g++ -c utils.cpp -fno-omit-frame-pointer -O2

clean:
	rm -r *.o