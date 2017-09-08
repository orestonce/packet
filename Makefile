packettest	: main.o rpcdata.o packet.o
	g++ main.o rpcdata.o packet.o -o packettest

main.o	: main.cpp rpcdata.h packet.h localmacro.h
	g++ -Wall -g -c -std=c++0x main.cpp

rpcdata.o	: rpcdata.cpp rpcdata.h packet.h localmacro.h
	g++ -Wall -g -c -std=c++0x rpcdata.cpp

packet.o	: packet.cpp packet.h localmacro.h
	g++ -Wall -g -c -std=c++0x packet.cpp
	
clean	:
	rm *.o packettest core.* -f
