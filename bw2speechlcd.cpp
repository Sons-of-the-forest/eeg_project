#include <iostream>
#include <fstream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <Python.h>
#define LCD_CHR         1			// RS = 1 = Character.
#define LCD_CMD         0			// RS = 0 = Command.
#define LCD_RW          0b0010		// RW = 0 = Write; RW 1 = Read. However we should NOT use Read mode.
#define LCD_EN          0b0100		// Enable bit. Must be pulse HIGH for at least 450ns after sending 4 bits.
#define LCD_BACKLIGHT   0b1000		// Backlight control. 0 = OFF, 1 = ON.

#define LINE1 0x80		  // 1st line
#define LINE2 0xC0		  // 2nd line

// function propotypes
void lcd_setAddr(int fd);
void lcd_init(int addr);

void lcd_byte(int bits, int mode);		  // send a byte of data
void lcd_toggle_enable(int bits);		  // toggle Enable bit
void typeInt(int i);					  // print integer
void typeFloat(float myFloat);			  // print float
void typeString(const char *s);			  // print string
void typeChar(char val);				  // print a char
void lcdLoc(int line);					  // move cursor
void ClrLcd(void);						  // clr LCD return home

typedef struct I2C16x2 {
	int fd;
	int addr;
} I2C16x2;

I2C16x2 lcd;
#define LCD_ADDR 0x27
#define INTERRUPT_PIN 27
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
void displayWordLCD(void);
string convertTextVN(string);
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
    lcd_init(LCD_ADDR);
    usleep(1000000);
    cout << "READY!" << endl;
    ClrLcd();
    typeString("READY!");
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
                                            string fileNameStr = "/home/pi/Desktop/eeg/eeg_project/dataOutput/data.csv";
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
                                            ClrLcd();
                                            typeString("Completed Writing");
                                            //Send data to server to predict and output speech
                                            predictAndSpeech();
                                            usleep(500000);
                                            displayWordLCD();
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

void switchInterrupt(void)
{
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 600)
    {
        // interrupt okay
        interruptNum += 1;
        printf("Button Pressed %d\n", interruptNum);
        ClrLcd();
        typeString("Button Pressed ");
        // printf("Button before %d\n", button);
        fflush(stdout);
        if (button == idleBtn)
        {
            cout << "PARSE" << endl;
            ClrLcd();
            typeString("PARSE ");
            button = parseBtn;
        }
        else if (button == parseBtn)
        {
            cout << "WRITE" << endl;
            ClrLcd();
            typeString("WRITE ");
            button = writeBtn;
        }
        else
        {
            button = idleBtn;
            cout << "OVERFLOW DATA. RESETTED BTN TO EDLE" << endl;
            ClrLcd();
            typeString("OVERFLOW DATA ");
        }
        // printf("Button after%d\n", button);
    }
    last_interrupt_time = interrupt_time;
}

int serRead(int serial)
{
    int ch;
    ch = serialGetchar(serial);
    // printf("%x\n", ch);
    return ch;
}

void predictAndSpeech(){
    string script="import pyttsx3\nimport requests\nimport pandas as pd\nimport json\n"
    "df = pd.read_csv('dataOutput/data.csv')\n"
    "list_object = df[' Brainwave Value'].to_list()\n"
    "response = requests.post('http://ac46-35-224-19-255.ngrok.io/api/predict', json={'list': list_object})\n"
    "try:\n"
    "   response = json.loads(response.text)\n"
    "   print(response['word'])\n"
    "   print(response['prob'])\n"
    "   output=max(response['prob'], key=lambda x:x[1])\n"
    "   file= open('temp.txt', 'w')\n"
    "   file.write(output[0])\n"
    "   file.close()\n"
    "   if output[1]>0.5:\n"
    "       engine=pyttsx3.init()\n"
    "       voices=engine.getProperty('voices')\n"
    "       engine.setProperty('voice',voices[70].id)\n"
    "       engine.say(output[0])\n"
    "       print(output[0])\n"
    "       engine.runAndWait()\n"
    "except:\n"
    "   print('SERVER ERROR')\n"
    "   file= open('temp.txt', 'w')\n"
    "   file.write('SERVER ERROR')\n"
    "   file.close()\n";
    PyRun_SimpleString(script.c_str());
    // Py_Finalize();
}

void displayWordLCD(){
    string line;
    ifstream myfile;
    myfile.open("temp.txt");
    if (myfile.is_open()) {
        if (getline(myfile, line)) {
            if(line!="SERVER ERROR"){
                line=convertTextVN(line);
            }
            ClrLcd();
            typeString(line.c_str());
        }
        myfile.close();
    } else {
        std::cout << "Unable to open file";
    }
}

string convertTextVN(string text){
    string newText;
    if(text=="Bệnh Viện"){
        newText="Benh Vien";
    }
    else if(text=="Cảm Ơn"){
        newText="Cam On";
    }
    else if(text=="Đất Nước"){
        newText="Dat Nuoc";
    }
    else if(text=="Tạm Biệt"){
        newText="Tam Biet";
    }
    else if(text=="Trường Học"){
        newText="Truong Hoc";
    }
    return newText;
}


// set up i2c device
void lcd_setAddr(int addr) {
	lcd.addr = addr;
	lcd.fd = wiringPiI2CSetup(addr);
}

void lcd_byte(int bits, int mode) {

	int bits_high;
	int bits_low;
	// uses the two half byte writes to LCD
	bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
	bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

	// High bits
	wiringPiI2CWrite(lcd.fd, bits_high);
	lcd_toggle_enable(bits_high);

	// Low bits
	wiringPiI2CWrite(lcd.fd, bits_low);
	lcd_toggle_enable(bits_low);
}

void lcd_toggle_enable(int bits) {
	// Toggle enable pin on LCD display
	wiringPiI2CWrite(lcd.fd, (bits | LCD_EN));		   // EN HIGH
	delayMicroseconds(500);							   // HIGH for 500us
	wiringPiI2CWrite(lcd.fd, (bits & ~LCD_EN));		   // EN LOW
	delayMicroseconds(500);							   // Data needs >37us to settle, use 500us to make sure
}

void lcd_init(int addr) {
	lcd_setAddr(addr);				// set lcd
	delay(15);						// (1) wait >15ms for LCD to power On
	lcd_byte(0x33, LCD_CMD);		// (2) and (3)
	lcd_byte(0x32, LCD_CMD);		// (4) and (5)

	// initial commands
	lcd_byte(0x28, LCD_CMD);		// 4-bit mode, 2 line, small font size
	lcd_byte(0x0C, LCD_CMD);		// Display ON, Cursor OFF, Cursor blink OFF
	lcd_byte(0x06, LCD_CMD);		// Cursor move direction left to right, no display shift
	lcd_byte(0x01, LCD_CMD);		// Clear display
}

// clear lcd and move cursor to home location 0x80 (LINE 1 ROW 0)
void ClrLcd(void) {
	lcd_byte(0x01, LCD_CMD);
}

// move cursor to location on LCD
// example: to go to column 3 of row 1 calls: lcdLoc(LINE1+3). Column start from 0
void lcdLoc(int line) {
	lcd_byte(line, LCD_CMD);
}

// print char to LCD at current position
void typeChar(char val) {
	lcd_byte(val, LCD_CHR);
}

// print a string of any length to LCD (no text wrap)
void typeString(const char *s) {
	while (*s)
		lcd_byte(*(s++), LCD_CHR);
}

// print a float number, up to 4 digits before decimal point and 2 digits after decimal point
void typeFloat(float myFloat) {
	char buffer[20];
	sprintf(buffer, "%4.2f", myFloat);
	typeString(buffer);
}

// print integer as string
void typeInt(int i) {
	char array1[20];
	sprintf(array1, "%d", i);
	typeString(array1);
}
