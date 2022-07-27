fp: main.o lib.o utils.o finger_print.o Executive.o config.o
	g++ -std=c++11 -g -Wall main.o lib.o utils.o finger_print.o Executive.o config.o -o fp -lwiringPi -pthread

main.o: main.cpp Executive.h
	g++ -std=c++11 -g -Wall -c main.cpp
	
Executive.o: Executive.h Executive.cpp sensor_model/lib/lib.h configuration/config.h
	g++ -std=c++11 -g -Wall -c Executive.cpp
	
config.o: configuration/config.h configuration/config.cpp sensor_model/lib/lib.h
	g++ -std=c++11 -g -Wall -c configuration/config.cpp
	
finger_print.o: sensor_model/finger_print.h sensor_model/finger_print.cpp sensor_model/lib/lib.h
	g++ -std=c++11 -g -Wall -c sensor_model/finger_print.cpp

lib.o: sensor_model/lib/lib.h sensor_model/lib/lib.cpp 
	g++ -std=c++11 -g -Wall -c sensor_model/lib/lib.cpp
	
utils.o: configuration/utils/utils.h configuration/utils/utils.cpp 
	g++ -std=c++11 -g -Wall -c configuration/utils/utils.cpp

clean: 
	rm *.o fp
