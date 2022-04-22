fp: main.o Executive.o FingerPrinter.o
	g++ -std=c++11 -g -Wall main.o Executive.o FingerPrinter.o -o fp -lwiringPi -pthread

main.o: main.cpp Executive.h FingerPrinter.h
	g++ -std=c++11 -g -Wall -c main.cpp

Executive.o: Executive.h Executive.cpp FingerPrinter.cpp
	g++ -std=c++11 -g -Wall -c Executive.cpp
	
FingerPrinter.o: FingerPrinter.h FingerPrinter.cpp
	g++ -std=c++11 -g -Wall -c FingerPrinter.cpp
	
clean: 
	rm *.o fp
