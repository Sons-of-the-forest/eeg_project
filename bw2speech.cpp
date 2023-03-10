#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <Python.h>
// #include <wiringPiI2C.h>
#define LCD_ADDR 0x3f
// #include <i2c1602.h>
// I2C16x2 lcd;
// #include "soft_lcd.h"
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
void predictAndSpeech(void);
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
    //Initialize Python
    Py_Initialize();
    PyObject* voiceModule=PyImport_ImportModule("pyttsx3");
    PyObject* requests=PyImport_ImportModule("requests");
    PyObject* pd=PyImport_ImportModule("pandas");
    PyObject* engine=PyObject_GetAttrString(voiceModule, "engine");
    //Initialize Python
    // I2cControl *i2c = new I2cControl(1);
    // LcdDriver lcd(0x27, i2c);
    // lcd_t *lcd_create(int scl, int sda, int addr, int lines);
    lcd_init(LCD_ADDR);
    usleep(1000000);
    cout << "READY!" << endl;
    // typeString("Hello World");
    // lcd.lcdSendCommand(LCD_BEG_LINE_1);
    // lcd.lcdString("READY");
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
                                            string fileNameStr = "dataOutput/data.csv";
                                            const char *fileName = fileNameStr.c_str();
                                            usleep(1000000);
                                            // sprintf(fileName, "dataRaw/tamws/tamws%d.csv", dataNum);
                                            fptr = fopen(fileName, "w");
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
                                            //Send data to server to predict and output speech
                                            predictAndSpeech();
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

void predictAndSpeech(){
    // Py_Initialize();
    // PyObject* voiceModule=PyImport_ImportModule("pyttsx3");
    // PyObject* requests=PyImport_ImportModule("requests");
    // PyObject* pd=PyImport_ImportModule("pandas");
    // PyObject* engine=PyObject_GetAttrString(voiceModule, "engine");
    string script="import pyttsx3\nimport requests\nimport pandas as pd\nimport json\n"
    "df = pd.read_csv('dataOutput/data.csv')\n"
    "list_object = df[' Brainwave Value'].to_list()\n"
    "response = requests.post('http://bab6-34-87-23-195.ngrok.io/api/predict', json={'list': list_object})\n"
    "response = json.loads(response.text)\n"
    "print(response['word'])\n"
    "print(response['prob'])\n"
    "output=max(response['prob'], key=lambda x:x[1])\n"
    "if output[1]>0.5:\n"
    "   engine=pyttsx3.init()\n"
    "   voices=engine.getProperty('voices')\n"
    "   engine.setProperty('voice',voices[70].id)\n"
    "   engine.say(output[0])\n"
    "   print(output[0])\n"
    "   engine.runAndWait()";
    PyRun_SimpleString(script.c_str());
    // Py_Finalize();
}