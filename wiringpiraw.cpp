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
int dataNum = 50; // Increase this number to write new files
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
    // KHOI TAO GPIO start//
    wiringPiSetupGpio();
    pinMode(INTERRUPT_PIN, INPUT);
    wiringPiISR(INTERRUPT_PIN, INT_EDGE_FALLING, &switchInterrupt);
    usleep(1000000);
    if ((fd = serialOpen("/dev/ttyS0", 57600)) < 0)
    {
        fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
    }
    usleep(1000000);
    cout << "READY!" << endl;
    // KHOI TAO GPIO end//
    while (1)
    {
        // CHECK DU LIEU GUI TOI SERIAL //
        if (serialDataAvail(fd))
        {
            // CHECK DU LIEU DUNG DINH DANG start //
            if (serRead(fd) == 0xAA)
            {
                if (serRead(fd) == 0xAA)
                {
                    int pLength = serRead(fd);
                    if (pLength == 0x04)
                    {
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
                            for (int i = 0; i < pLength; i++)
                            {
                                if (payloadData[i] == 0x80)
                                {
                                    if (payloadData[i + 1] == 0x02)
                                    {
                                        // CHECK DU LIEU DUNG DINH DANG end //
                                        // CHECK TRANG THAI = THU DU LIEU ? //
                                        if (button == parseBtn)
                                        {
                                            // LUU DU LIEU VAO BIEN TAM  start//
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
                                            // LUU DU LIEU VAO BIEN TAM  end//
                                        }
                                        // CHECK TRANG THAI = GHI DU LIEU ? //
                                        if (button == writeBtn)
                                        {
                                            // GHI DU LIEU RA FILE start//
                                            dataNum += 1;
                                            FILE *fptr;
                                            string fileNameStr = "dataRaw/tambiet_new/tambiet_new" + to_string(dataNum) + ".csv";
                                            const char *fileName = fileNameStr.c_str();
                                            usleep(1000000);
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
                                            // GHI DU LIEU RA FILE end//
                                            // CHUYEN TRANG THAI -> NGHI //
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
    }
    return 0;
}
//HAM NGAT NUT BAM start//
void switchInterrupt(void)
{
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 600)
    {
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
}
//HAM NGAT NUT BAM end//

int serRead(int serial)
{
    int ch;
    ch = serialGetchar(serial);
    // printf("%x\n", ch);
    return ch;
}