packettest	: main.o rpcdata-server.o packet.o
	g++ main.o rpcdata-server.o packet.o -o packettest

main.o	: main.cpp rpcdata-server.h packet.h localmacro.h
	g++ -Wall -g -c -std=c++0x main.cpp

rpcdata-server.o	: rpcdata-server.cpp rpcdata-server.h packet.h localmacro.h
	g++ -Wall -g -c -std=c++0x rpcdata-server.cpp

packet.o	: packet.cpp packet.h localmacro.h
	g++ -Wall -g -c -std=c++0x packet.cpp
	
clean	:
	rm *.o packettest core.* -f
