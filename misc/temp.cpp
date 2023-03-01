/*
 通过UART串口显示信号值、注意力及放松度的值
 */
#define BAUDRATE 9600
#define DEBUGOUTPUT 0

//校验相关变量
int   generatedChecksum = 0;
byte  checksum = 0; 
int   trueCheckMessage=0;
int   betaMessage=0;
int   totalMessage=0;
int   trueLengthMessage=0;
int   lengthMessage0x20=0;

//接收数据长度和数据数组
byte  payloadLength = 0;
//总共接收32个自己的数据

//需要读取的信息变量
byte signalquality = 0;//信号质量
byte attention = 0;    //注意力值
byte meditation = 0;   //放松度值
byte lowBeta=0;
byte highBeta=0;
byte lowBeta1=0;
byte highBeta1=0;
byte lowBeta2=0;
byte highBeta2=0;
byte lowBeta3=0;
byte highBeta3=0;
byte poorSignal=0;


void setup() 
{
  Serial.begin(BAUDRATE); 
  Serial.print(0x00);
  delay(10000);
}

//从串口读取一个字节数据
byte ReadOneByte() 
{
  int ByteRead;
  while(!Serial.available());
  ByteRead = Serial.read();
  return ByteRead;//返回读到的字节
}

//读取串口数据
void read_serial_data()
{   
    if(ReadOneByte() == 0xAA)
    {
      if(ReadOneByte() == 0xAA)
      {
        totalMessage+=1;
        payloadLength = ReadOneByte();
        while (1){
          if(payloadLength==0xAA){
            payloadLength = ReadOneByte();
          }
          else{
            break;
          }
        }
        if(payloadLength < 0xAA)
        {
          byte payloadData[payloadLength] = {0};
          trueLengthMessage+=1;
          generatedChecksum = 0;      
          for(int i = 0; i < payloadLength; i++)
          {  
            payloadData[i] = ReadOneByte();
            generatedChecksum += payloadData[i];
          }         
          checksum = ReadOneByte();
          generatedChecksum = (~generatedChecksum)&0xff;        
          if(checksum == generatedChecksum)
          { 
            trueCheckMessage+=1;
            for(int i = 0; i < payloadLength; i++)
          {  
            if(payloadData[i] == 0x83){
              if(payloadData[i+1] == 0x18){
                if(payloadLength==0x20){
                  lengthMessage0x20+=1; //check msg between length and beta
                }
                betaMessage+=1;
                lowBeta1=payloadData[i+2];
                lowBeta2=payloadData[i+3];
                lowBeta3=payloadData[i+4];
                highBeta1=payloadData[i+5];
                highBeta2=payloadData[i+6];
                highBeta3=payloadData[i+7];
                lowBeta=(payloadData[16]<<16)|(payloadData[17]<<8)|(payloadData[18]);
                highBeta=(payloadData[19]<<16)|(payloadData[20]<<8)|(payloadData[21]);
                Serial.print("Low Beta: ");
                Serial.print(lowBeta, DEC);
                Serial.print(" High Beta: ");
                Serial.print(highBeta, DEC);
                Serial.print("\n"); 
              }
            }
          }    
        }
      }
    }
  }
  Serial.print("total Message: "); 
  Serial.print(totalMessage, DEC); 
  Serial.print("\n"); 
  Serial.print("true Length Message : "); 
  Serial.print(trueLengthMessage, DEC); 
  Serial.print("\n"); 
  Serial.print("true Check Message: "); 
  Serial.print(trueCheckMessage, DEC); 
  Serial.print("\n");
  Serial.print("beta Message: "); 
  Serial.print(betaMessage, DEC); 
  Serial.print("\n");
  Serial.print("lengthMessage0x20: "); 
  Serial.print(lengthMessage0x20, DEC); 
  Serial.print("\n"); 
}

//主循环
void loop() 
{
  read_serial_data();//读取串口数据 
}