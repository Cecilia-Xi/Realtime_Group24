fp: main.o lib.o 
	g++ -std=c++11 -g -Wall main.o lib.o -o fp -lwiringPi -pthread

main.o: main.cpp lib.h
	g++ -std=c++11 -g -Wall -c main.cpp

lib.o: lib.h lib.cpp 
	g++ -std=c++11 -g -Wall -c lib.cpp

clean: 
	rm *.o fp
