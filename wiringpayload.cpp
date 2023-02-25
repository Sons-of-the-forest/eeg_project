#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define INTERRUPT_PIN 27
// #define BUTTON_VCC 17
using namespace std;
int generatedChecksum=0;
int checksum=0;
int lowBeta=0;
int highBeta=0;
// int lowBeta1=0;
// int highBeta1=0;
// int lowBeta2=0;
// int highBeta2=0;
// int lowBeta3=0;
// int highBeta3=0;
int delta=0;
int theta=0;
int lowAlpha=0;
int highAlpha=0;
int lowGamma=0;
int midGamma=0;
int attention=0;
int meditation=0;
int poorSignal=-1;
int lowBetaData[100]={};
int highBetaData[100]={};
int alphaData[100]={};
int thetaData[100]={};
int deltaData[100]={};
int lowAlphaData[100]={};
int highAlphaData[100]={};
int lowGammaData[100]={};
int midGammaData[100]={};
int attentionData[100]={};
int meditationData[100]={};
int poorSignalData[100]={};
int   trueCheckMessage=0;
int   betaMessage=0;
int   totalMessage=0;
int   trueLengthMessage=0;
int interruptNum=0;
int dataNum=0;
unsigned long last_interrupt_time=0;
enum btn {idleBtn, parseBtn, writeBtn};
btn button= idleBtn;
void switchInterrupt(void);
int serRead(int);
int main(){
    int fd;
    int index=0;
    wiringPiSetupGpio();
    pinMode(INTERRUPT_PIN, INPUT);
    wiringPiISR(INTERRUPT_PIN,INT_EDGE_FALLING,&switchInterrupt);
    // pinMode(BUTTON_VCC, OUTPUT);
    // digitalWrite(BUTTON_VCC, HIGH);
    usleep(1000000);
    if((fd=serialOpen("/dev/ttyS0", 57600))<0){
        fprintf(stderr, "Unable to open serial device: %s\n", strerror (errno));
    }
    // char data[]="0x00";
    // int length=strlen(data);
    // serialPrintf(fd, data, length);
    usleep(1000000);
    cout<<"READY!"<<endl;
    while(1){
        if(serialDataAvail(fd)){
            printf("%x \n", serRead(fd));
        }
        // printf("total Message: %d\ntrue Length Message : %d\ntrue Check Message: %d\nbeta Message: %d\n", totalMessage, trueLengthMessage, trueCheckMessage, betaMessage); 
        // printf("true Length Message : %d\n", trueLengthMessage); 
        // printf("true Check Message: %d\n", trueCheckMessage); 
        // printf("beta Message: %d\n", betaMessage); 
        // fflush(stdout);
    }
return 0;
}

void switchInterrupt(void){
    unsigned long interrupt_time=millis();
    if (interrupt_time-last_interrupt_time>600){
        //interrupt okay
        interruptNum+=1;
        printf("Button Pressed %d\n", interruptNum);
        fflush(stdout);
        if (button==idleBtn){
        cout<<"PARSE"<<endl;
        button=parseBtn;
        }
        else if(button ==parseBtn){
        cout<<"WRITE"<<endl;
        button=writeBtn;
        }
    }
    last_interrupt_time=interrupt_time;
    // while (!digitalRead(INTERRUPT_PIN)){
    //     //do nothing//
    // }
}

int serRead(int serial){
    int ch;
    ch=serialGetchar(serial);
    // printf("%x\n", ch);
    return ch;
}