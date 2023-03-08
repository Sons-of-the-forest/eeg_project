#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define INTERRUPT_PIN 27
// #define BUTTON_VCC 17
using namespace std;
int generatedChecksum = 0;
int checksum = 0;
int lowBeta = 0;
int highBeta = 0;
// int lowBeta1=0;
// int highBeta1=0;
// int lowBeta2=0;
// int highBeta2=0;
// int lowBeta3=0;
// int highBeta3=0;
int delta = 0;
int theta = 0;
int lowAlpha = 0;
int highAlpha = 0;
int lowGamma = 0;
int midGamma = 0;
int attention = 0;
int meditation = 0;
int poorSignal = -1;
int lowBetaData[100] = {};
int highBetaData[100] = {};
int alphaData[100] = {};
int thetaData[100] = {};
int deltaData[100] = {};
int lowAlphaData[100] = {};
int highAlphaData[100] = {};
int lowGammaData[100] = {};
int midGammaData[100] = {};
int attentionData[100] = {};
int meditationData[100] = {};
int poorSignalData[100] = {};
int trueCheckMessage = 0;
int betaMessage = 0;
int totalMessage = 0;
int trueLengthMessage = 0;
int interruptNum = 0;
int dataNum = 0;
unsigned long last_interrupt_time = 0;
enum btn
{
    idleBtn,
    parseBtn,
    writeBtn
};
btn button = idleBtn;
void switchInterrupt(void);
int serRead(int);
int main()
{
    int fd;
    int index = 0;
    wiringPiSetupGpio();
    pinMode(INTERRUPT_PIN, INPUT);
    wiringPiISR(INTERRUPT_PIN, INT_EDGE_FALLING, &switchInterrupt);
    // pinMode(BUTTON_VCC, OUTPUT);
    // digitalWrite(BUTTON_VCC, HIGH);
    usleep(1000000);
    if ((fd = serialOpen("/dev/ttyS0", 9600)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
    }
    char data[] = "0x00";
    int length = strlen(data);
    serialPrintf(fd, data, length);
    usleep(1000000);
    cout << "READY!" << endl;
    while (1)
    {
        if (serialDataAvail(fd))
        {
            if (serRead(fd) == 0xAA)
            {
                if (serRead(fd) == 0xAA)
                {
                    // printf("Check");
                    // fflush(stdout);
                    totalMessage += 1;
                    int pLength = serRead(fd);
                    if (pLength == 0x20)
                    {
                        trueLengthMessage += 1;
                        int payloadData[pLength] = {0};
                        generatedChecksum = 0;
                        for (int i = 0; i < pLength; i++)
                        {
                            payloadData[i] = serRead(fd);
                            generatedChecksum += payloadData[i];
                        }
                        checksum = serRead(fd);
                        generatedChecksum = (~generatedChecksum) & 0xff;
                        if (checksum == generatedChecksum)
                        {
                            trueCheckMessage += 1;
                            for (int i = 0; i < pLength; i++)
                            {
                                if (payloadData[i] == 0x83)
                                {
                                    if (payloadData[i + 1] == 0x18)
                                    {
                                        // printf("Low Beta1: %d\nHigh Beta1: %d\n", lowBeta1, highBeta1);
                                        // printf("Low Beta: %d\nHigh Beta: %d\n", lowBeta, highBeta);
                                        // fflush(stdout);
                                        // printf("High Beta: %x\n", highBeta);
                                        if (button == parseBtn)
                                        {

                                            // lowBeta1=payloadData[i+14];
                                            // lowBeta2=payloadData[i+15];
                                            // lowBeta3=payloadData[i+16];
                                            // highBeta1=payloadData[i+17];
                                            // highBeta2=payloadData[i+18];
                                            // highBeta3=payloadData[i+19];
                                            poorSignal = payloadData[i - 1];
                                            delta = (payloadData[i + 2] << 16) | (payloadData[i + 3] << 8) | (payloadData[i + 4]);
                                            theta = (payloadData[i + 5] << 16) | (payloadData[i + 6] << 8) | (payloadData[i + 7]);
                                            lowAlpha = (payloadData[i + 8] << 16) | (payloadData[i + 9] << 8) | (payloadData[i + 10]);
                                            highAlpha = (payloadData[i + 11] << 16) | (payloadData[i + 12] << 8) | (payloadData[i + 13]);
                                            lowBeta = (payloadData[i + 14] << 16) | (payloadData[i + 15] << 8) | (payloadData[i + 16]);
                                            highBeta = (payloadData[i + 17] << 16) | (payloadData[i + 18] << 8) | (payloadData[i + 19]);
                                            lowGamma = (payloadData[i + 20] << 16) | (payloadData[i + 21] << 8) | (payloadData[i + 22]);
                                            midGamma = (payloadData[i + 23] << 16) | (payloadData[i + 24] << 8) | (payloadData[i + 25]);
                                            attention = payloadData[i + 27];
                                            meditation = payloadData[i + 29];
                                            deltaData[index] = delta;
                                            thetaData[index] = theta;
                                            lowAlphaData[index] = lowAlpha;
                                            highAlphaData[index] = highAlpha;
                                            lowBetaData[index] = lowBeta;
                                            highBetaData[index] = highBeta;
                                            lowGammaData[index] = lowGamma;
                                            midGammaData[index] = midGamma;
                                            attentionData[index] = attention;
                                            meditationData[index] = meditation;
                                            poorSignalData[index] = poorSignal;
                                            index++;
                                        }
                                        if (button == writeBtn)
                                        {
                                            dataNum += 1;
                                            FILE *fptr;
                                            fptr = fopen("data/output.csv", "a+"); // change output file name to word name
                                            if (fptr == NULL)
                                            {
                                                printf("Error opening file.\n");
                                                return 1;
                                            }
                                            fprintf(fptr, "Poor Signal, Delta, Theta, Low Alpha, High Alpha, Low Beta, High Beta, Low Gamma, Mid Gamma, Attention, Meditation, %d\n", dataNum);
                                            for (int i = 0; i < index; i++)
                                            {
                                                fprintf(fptr, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d \n", poorSignalData[i], deltaData[i], thetaData[i], lowAlphaData[i], highAlphaData[i], lowBetaData[i], highBetaData[i],
                                                        lowGammaData[i], midGammaData[i], attentionData[i], meditationData[i]);
                                            }
                                            // fprintf(fptr, "High Beta\n");
                                            // for(int i =0;i<sizeof(highBetaData);i++){
                                            //     fprintf(fptr, "%d\n", highBetaData[i]);
                                            // }
                                            fclose(fptr);
                                            button = idleBtn;
                                            index = 0;
                                            printf("Complete Writing %d\n", dataNum);
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // printf("total Message: %d\ntrue Length Message : %d\ntrue Check Message: %d\nbeta Message: %d\n", totalMessage, trueLengthMessage, trueCheckMessage, betaMessage);
        // printf("true Length Message : %d\n", trueLengthMessage);
        // printf("true Check Message: %d\n", trueCheckMessage);
        // printf("beta Message: %d\n", betaMessage);
        // fflush(stdout);
    }
    return 0;
}

void switchInterrupt(void)
{
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 600)
    {
        // interrupt okay
        interruptNum += 1;
        printf("Button Pressed %d\n", interruptNum);
        fflush(stdout);
        if (button == idleBtn)
        {
            cout << "PARSE" << endl;
            button = parseBtn;
        }
        else if (button == parseBtn)
        {
            cout << "WRITE" << endl;
            button = writeBtn;
        }
        else
        {
            button = idleBtn;
            cout << "OVERFLOW DATA. RESETTED BTN TO EDLE" << endl;
        }
    }
    last_interrupt_time = interrupt_time;
    // while (!digitalRead(INTERRUPT_PIN)){
    //     //do nothing//
    // }
}

int serRead(int serial)
{
    int ch;
    ch = serialGetchar(serial);
    // printf("%x\n", ch);
    return ch;
}