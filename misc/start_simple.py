'''
UART communication on Raspberry Pi using Pyhton
http://www.electronicwings.com
'''
import serial
from time import sleep
SYNC = 0xAA
EXCODE=0x55
notEEG=0
def serRead():
    return int(serial.Serial ("/dev/ttyS0", 9600).read().hex(),16)

def parsePayload(mainPayload):
    lowBeta=-1
    highBeta=-1
    if mainPayload[2]==0x83 and mainPayload[3]==0x18:
        lowBeta=(mainPayload[16]<<16)|(mainPayload[17]<<8)|(mainPayload[18])
        highBeta=(mainPayload[19]<<16)|(mainPayload[20]<<8)|(mainPayload[21])
    else:
        notEEG+=1
        print("Not egg"+str(notEEG))
    print(" Low Beta: "+ str(lowBeta))
    print(" High Beta: "+ str(highBeta))

def main():         
    ser = serial.Serial ("/dev/ttyS0", 9600)    #Open port with baud rate
    cmd=0x00
    ser.write(str(cmd).encode())
    sleep(2)
    while True:
        #Synchronize on [SYNC] byte
        if serRead()!=SYNC:
            continue
        if serRead()!=SYNC:
            continue
        pLength=serRead()
        if pLength==0x20:
        #Collect [PAYLOAD...] bytes & calculate checksum
            mainPayload=[]
            checksum=0
            print("Payload length "+str(pLength))
            for i in range(pLength):
                temp=serRead()
                mainPayload.append(temp)
                checksum+=temp
            # checksum&=0xFF
            checksum=(~checksum)&0xFF
            #Parse checksum byte and compare with calculated checksum byte
            checksumByte=serRead()
            print("checksumByte " + str(checksumByte)+" checksum "+str(checksum))
            if checksumByte!=checksum:
                continue
            parsePayload(mainPayload) 


if __name__=="__main__":
    main()
    

               
                
                
        
    
        
