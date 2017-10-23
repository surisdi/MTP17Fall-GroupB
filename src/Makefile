


default: main

main: Compressor.o Protocol.o Socket.o comms.o control.o utils.o main.cpp Compressor.hpp Protocol.hpp Socket.hpp comms.hpp control.hpp utils.hpp 
	g++ -L home/pi/RF24/ -lrf24 -o ../main main.cpp Compressor.o Protocol.o Socket.o comms.o control.o utils.o


Compressor.o: Compressor.cpp Compressor.hpp
	g++ -c Compressor.cpp

Socket.o: Socket.cpp Socket.hpp
	g++ -L home/pi/RF24/ -lrf24 -c Socket.cpp


comms.o: comms.cpp comms.hpp
	g++ -c comms.cpp

utils.o: utils.cpp utils.hpp
	g++ -c utils.cpp


clean:
	rm -r *.o