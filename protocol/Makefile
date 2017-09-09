genrpc	: main.o tinyxml2.o genrpc.o
	g++ main.o tinyxml2.o genrpc.o -g -o genrpc

main.o	: main.cpp genrpc.h tinyxml2.h
	g++ -g -c -std=c++0x main.cpp

tinyxml2.o	: tinyxml2.cpp tinyxml2.h
	g++ -g -c -std=c++0x tinyxml2.cpp

genrpc.o	: genrpc.cpp genrpc.h tinyxml2.h
	g++ -g -c -std=c++0x genrpc.cpp

clean	:
	rm -f genrpc *.o core.*
