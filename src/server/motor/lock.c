#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>

int initwiringPiSetup()
        if (ret == -1）{
                printf("Initial Failed\n");
        }
}
void initPin()
{
        pinMode(7,OUTPUT);//set output pin at 7
}

int main(int argc,char const*argv[]){

        initwiringPiSetup();
        initPin();


        while(1){
                initPin();
                printf("Door is open,unlock after 3s\n");
                sleep(3);
                pinMode(7,INPUT);
                printf("Door is open,lock after 2s\n");
                sleep(2);
        }
                return 0;
}
