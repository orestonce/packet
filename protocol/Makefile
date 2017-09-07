genrpc	: main.o tinyxml2.o
	g++ main.o tinyxml2.o -g -o genrpc

main.o	: main.cpp tinyxml2.h
	g++ -g -c -std=c++0x main.cpp

tinyxml2.o	: tinyxml2.cpp tinyxml2.h
	g++ -g -c -std=c++0x tinyxml2.cpp

clean	:
	rm -f a.out *.o core.*
