'''
UART communication on Raspberry Pi using Pyhton
http://www.electronicwings.com
'''
import serial
from time import sleep
SYNC = 0xAA
EXCODE=0x55
def main():
    ser = serial.Serial ("/dev/ttyS0", 9600)    #Open port with baud rate
    while True:
        while True:
            #Synchronize on [SYNC] byte
            if ser.read()!=SYNC:
                continue
            if ser.read()!=SYNC:
                continue
            #Parse [PLENGTH] byte
            while True:
                pLength=ser.read()
                if pLength!=170:
                    break
            if pLength>169:
                continue
            #Collect [PAYLOAD...] bytes & calculate checksum
            mainPayload=[]
            checksum=0
            for i in range(pLength):
                mainPayload[i]=ser.read()
                checksum+=mainPayload[i]
            checksum&=0xFF
            checksum=~checksum&0xFF
            #Parse checksum byte and compare with calculated checksum byte
            checksumByte=ser.read()
            if checksumByte!=checksum:
                continue
            parsePayload(mainPayload,pLength) 


if __name__==")__main___":
    main()
    
def parsePayload(mainPayload, pLength):
    byteParsed=0
    poorSignal=-1
    attention=-1
    meditation=-1
    blink=-1
    rawData=-1
    eegData=-1
    delta=-1
    theta=-1
    lowAlpha=-1
    highAlpha=-1
    lowBeta=-1
    highBeta=-1
    lowGamma=-1
    midGamma=-1
    unknownByte=False
    while byteParsed < pLength:
        # Parse extendedCode, code, length
        extendedCodeLevel=0
        while mainPayload[bytesParsed]==EXCODE:
            extendedCodeLevel+=1
            byteParsed+=1
        code = mainPayload[bytesParsed]
        if code == 0x02:
            length=1
            bytesParsed+=1
            poorSignal=mainPayload[bytesParsed]
        elif code== 0x04:
            length=1
            bytesParsed+=1
            attention=mainPayload[bytesParsed]
        elif code == 0x05:
            length=1
            bytesParsed+=1
            meditation=mainPayload[bytesParsed]
        elif code== 0x16:
            length=1
            bytesParsed+=1
            blink=mainPayload[bytesParsed]
        elif code == 0x80:
            bytesParsed+=1
            length=mainPayload[bytesParsed]
            rawData=0
            for i in range(length):
                rawData=rawData<<8
                bytesParsed+=1
                rawData=rawData|mainPayload[bytesParsed]
            print("rawData: " + rawData + "rawData Length: "+ length+"/n")
        elif code== 0x83:
            bytesParsed+=1
            length=mainPayload[bytesParsed]
            delta=(mainPayload[bytesParsed+1]<<16)|(mainPayload[bytesParsed+2]<<8)|mainPayload[bytesParsed+3]
            theta=(mainPayload[bytesParsed+4]<<16)|(mainPayload[bytesParsed+5]<<8)|mainPayload[bytesParsed+6]
            lowAlpha=(mainPayload[bytesParsed+7]<<16)|(mainPayload[bytesParsed+8]<<8)|mainPayload[bytesParsed+9]
            highAlpha=(mainPayload[bytesParsed+10]<<16)|(mainPayload[bytesParsed+11]<<8)|mainPayload[bytesParsed+12]
            lowBeta=(mainPayload[bytesParsed+13]<<16)|(mainPayload[bytesParsed+14]<<8)|mainPayload[bytesParsed+15]
            highBeta=(mainPayload[bytesParsed+16]<<16)|(mainPayload[bytesParsed+17]<<8)|mainPayload[bytesParsed+18]
            lowGamma=(mainPayload[bytesParsed+19]<<16)|(mainPayload[bytesParsed+20]<<8)|mainPayload[bytesParsed+21]
            midGamma=(mainPayload[bytesParsed+22]<<16)|(mainPayload[bytesParsed+23]<<8)|mainPayload[bytesParsed+24]
            bytesParsed+=24
            print("lowBeta: " + lowBeta + "highBeta: "+ highBeta+"/n")
        else:
            unknownByte=True
               
                
                
        
    
        
