#include <wiringPi.h>
#include <iostream>
#include <wiringSerial.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
using namespace std;

int main(void)


{

    wiringPiSetupGpio();

    pinMode(22, OUTPUT);

    digitalWrite(22, HIGH);
    while(1){

        printf("while");
        usleep(1000000);
        

    }

    return 0;
}