fp: main.o Executive.o AS608.o Utils.o
	g++ -std=c++11 -g -Wall main.o Executive.o AS608.o Utils.o -o fp -lwiringPi

main.o: main.cpp Executive.h
	g++ -std=c++11 -g -Wall -c main.cpp

Executive.o: Executive.h Executive.cpp Utils.h AS608.h 
	g++ -std=c++11 -g -Wall -c Executive.cpp
	
AS608.o: AS608.h AS608.cpp
	g++ -std=c++11 -g -Wall -c AS608.cpp 

Utils.o: Utils.h Utils.cpp
	g++ -std=c++11 -g -Wall -c Utils.cpp 

clean: 
	rm *.o fp
