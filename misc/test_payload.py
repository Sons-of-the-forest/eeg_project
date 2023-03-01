'''
UART communication on Raspberry Pi using Pyhton
http://www.electronicwings.com
'''
import serial
from time import sleep
import time
SYNC = 0xAA
EXCODE=0x55
BAUDRATE=57600
notEEG=0
def serRead():
    return int(serial.Serial ("/dev/ttyS0", BAUDRATE, inter_byte_timeout=0.1, timeout=1).read().hex(),16)

def main():         
    ser = serial.Serial ("/dev/ttyS0", BAUDRATE, inter_byte_timeout=0.1, timeout=1)    #Open port with baud rate
    # cmd=0x02
    # ser.write(str(cmd).encode())
    # print(str(cmd).encode())
    sleep(2)
    while True:
        print(str(hex(serRead())))


if __name__=="__main__":
    main()
    

               
                
                
        
    
        
