fp: main.o Executive.o lib.o utils.o
	g++ -std=c++11 -g -Wall main.o Executive.o lib.o utils.o -o fp -lwiringPi -pthread

main.o: main.cpp Executive.h
	g++ -std=c++11 -g -Wall -c main.cpp
	
Executive.o: Executive.h Executive.cpp lib.h utils.h
	g++ -std=c++11 -g -Wall -c Executive.cpp
	
lib.o: lib.h lib.cpp 
	g++ -std=c++11 -g -Wall -c lib.cpp
	
utils.o: utils.h utils.cpp 
	g++ -std=c++11 -g -Wall -c utils.cpp
	
clean: 
	rm *.o fp
