


default: main

main: Compressor.o Protocol.o Protocol_StopWait.o Protocol_GoBackN.o Socket.o utils.o main.cpp Compressor.hpp Protocol.hpp Socket.hpp utils.hpp 
	g++ -L home/pi/RF24/ -lrf24 -o ../main main.cpp Compressor.o Protocol.o Protocol_GoBackN.o Protocol_StopWait.o Socket.o utils.o -fno-omit-frame-pointer -O2 -pthread -lz

#main: Compressor.o Protocol.o Protocol_StopWait.o Protocol_GoBackN.o Socket.o utils.o main.cpp Compressor.hpp Protocol.hpp Socket.hpp utils.hpp 
#	g++ -g -std=gnu++11 -o ../main main.cpp Compressor.o Protocol.o Protocol_GoBackN.o Protocol_StopWait.o Socket.o utils.o -fno-omit-frame-pointer -O2 -pthread -lz

Protocol.o: Protocol.cpp Protocol.hpp
	g++ -g -std=gnu++11 -c Protocol.cpp

Protocol_GoBackN.o: Protocol_GoBackN.cpp Protocol.hpp
	g++ -g -std=gnu++11 -c Protocol_GoBackN.cpp

Protocol_StopWait.o: Protocol_StopWait.cpp Protocol.hpp
	g++ -g -std=gnu++11 -c Protocol_StopWait.cpp

Compressor.o: Compressor.cpp Compressor.hpp
	g++ -g -std=gnu++11 -c Compressor.cpp -lz -fno-omit-frame-pointer -O2

Socket.o: Socket.cpp Socket.hpp
	g++ -g -std=gnu++11 -c Socket.cpp -fno-omit-frame-pointer -O2

utils.o: utils.cpp utils.hpp
	g++ -g -std=gnu++11 -c utils.cpp -fno-omit-frame-pointer -O2

clean:
	rm -r *.o
	rm ../main
