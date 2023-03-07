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
int firstByte = 0;
int secondByte = 0;
int brainwaveValue = 0;
int trueCheckMessage = 0;
int betaMessage = 0;
int totalMessage = 0;
int trueLengthMessage = 0;
int interruptNum = 0;
int dataNum = 0; // Increase this number to write new files
int brainwavePayload[3000] = {0};
int firstBytePayload[3000] = {0};
int secondBytePayload[3000] = {0};
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
    usleep(1000000);
    if ((fd = serialOpen("/dev/ttyS0", 57600)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
    }
    // char data[]="0x00";
    // int length=strlen(data);
    // serialPrintf(fd, data, length);
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
                    // totalMessage+=1;
                    int pLength = serRead(fd);
                    if (pLength == 0x04)
                    {
                        // trueLengthMessage+=1;
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
                            // trueCheckMessage+=1;
                            for (int i = 0; i < pLength; i++)
                            {
                                if (payloadData[i] == 0x80)
                                {
                                    if (payloadData[i + 1] == 0x02)
                                    {
                                        if (button == parseBtn)
                                        {
                                            firstByte = payloadData[i + 2];
                                            secondByte = payloadData[i + 3];
                                            brainwaveValue = (firstByte * 256) + secondByte;
                                            if (brainwaveValue >= 32768)
                                            {
                                                brainwaveValue = brainwaveValue - 65536;
                                            }
                                            firstBytePayload[index] = firstByte;
                                            secondBytePayload[index] = secondByte;
                                            brainwavePayload[index] = brainwaveValue;
                                            index = index + 1;
                                            // printf("index %d\n", index)
                                        }
                                        if (button == writeBtn)
                                        {
                                            dataNum += 1;
                                            FILE *fptr;
                                            string fileNameStr = "dataRaw/baonhieu/baonhieu" + to_string(dataNum) + ".csv";
                                            const char *fileName = fileNameStr.c_str();
                                            usleep(1000000);
                                            // sprintf(fileName, "dataRaw/tamws/tamws%d.csv", dataNum);
                                            fptr = fopen(fileName, "a+");
                                            if (fptr == NULL)
                                            {
                                                printf("Error opening file.\n");
                                                return 1;
                                            }
                                            fprintf(fptr, "First Byte, Second Byte, Brainwave Value, %d\n", dataNum);
                                            for (int i = 0; i < index; i++)
                                            {
                                                fprintf(fptr, "%d, %d, %d \n", firstBytePayload[i], secondBytePayload[i], brainwavePayload[i]);
                                                // printf("%d, %d, %d \n", firstBytePayload[i], secondBytePayload[i], brainwavePayload[i]);
                                                fflush(stdout);
                                            }
                                            fclose(fptr);
                                            button = idleBtn;
                                            index = 0;
                                            printf("Completed Writing %d\n", dataNum);
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
        // printf("Button before %d\n", button);
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
        // printf("Button after%d\n", button);
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