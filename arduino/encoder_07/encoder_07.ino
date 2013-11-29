/* 
This Sketch is to create a I2C Slave device that will sequentially read all ADC channels via INTERRUPT
and store the values for quick use. 

This implementation will expose two QRD1114 IR Encoders (Left and Right Wheel)
as well as four IR Distance Sensors.
These are simple single encoder per wheel, and therefore has no concept of forward/reverse
if direction is required, we will toggle a bit in the Configuration register.


*/ 


// Define Arduino Pins used
#define Front_IRPin A0
#define Rear_IRPin  A1
#define LencoderPin A2            // Left Encoder PIN  Reads a QRD1114  IR sensor
#define RencoderPin A3            // Right Encoder PIN  Reads a QRD1114  IR sensor
#define SDAPIN      A4            // I2C SDA
#define SCLPIN      A5            // I2C SCL
#define Left_IRPin  A6
#define Right_IRPin A7



#define FORWARD 1
#define REVERSE 0
#define WHITE 1
#define BLACK 0
#define LEFT 65
#define RIGHT 66
#define THRESHOLD 700             // 650 seemed like a reasonable set point for transition.  Play with this...


unsigned long int lenc = 0, renc = 0, lpos = 0, rpos = 0;

unsigned long int lstate = 0, rstate = 0;
unsigned long int threshold = 600, ReportTime=0, now=0;
unsigned int front,rear,left,right;


#include <Wire.h>

#define  SLAVE_ADDRESS   0x29              // I2C slave address 
#define  REG_MAP_SIZE    16
#define  MAX_SENT_BYTES  3
#define  IDENTIFICATION  0x0D
#define  SMOOTH          20

 
byte registerMap[REG_MAP_SIZE];
byte receivedCommands[MAX_SENT_BYTES];    
/* three bytes is the most amount of data that would ever need to be sent from the master.  
The first byte sent from the master is always the register address.  
For our device, we only have two writable registers in our map and they are the configuration 
register and mode register.  The rest of the registers are read only so they can’t be written to.
*/


int Front_New = 0, Front_Avg = 0,Rear_New = 0, Rear_Avg = 0, Left_New = 0, Left_Avg = 0, Right_New = 0, Right_Avg = 0;

long Front_Tally = 0, Rear_Tally = 0, Left_Tally = 0, Right_Tally = 0;

byte newDataAvailable = 0;
byte Status = 0x0A;
byte useInterrupt = 1;
byte modeRegister = B0000000;
byte configRegister = 0;
byte zeroD = 0;
byte zeroE = 0;
byte zeroDData = B0000001;
byte zeroEData = 0;
 
// Register Map 
// Address 	Register Description
// 0x00 	Status Register
// 0x01 	Left ticks - MSB
// 0x02 	Left ticks - LSB
// 0x03 	Right ticks - MSB
// 0x04 	Right ticks - LSB
// 0x05 	Front Distance - MSB
// 0x06 	Front Distance - LSB
// 0x07 	Rear Distance - MSB
// 0x08 	Rear Distance - LSB
// 0x09 	Right Distance - MSB
// 0x0A 	Right Distance - LSB
// 0x0B 	Left Distance - MSB
// 0x0C 	Left Distance - LSB
// 0x0D 	Mode Register
// 0x0E 	Configuration Register
// 0x0F 	Identification Register

 
// Mode Register
// Most significant 4 bits reserved
// Bit 3 (B00001000)  is Left Wheel  Direction 0 is forward 1 is reverse
// Bit 2 (B00000100)  is Right Wheel Direction 0 is forward 1 is reverse
// Bit 1 (B00000010)  is reserved
// Bit 0 (B00000001)  is for counting reset mode  0 is continuous count 1 is reset count after every master read
 

void setup() { 

  pinMode(A0, INPUT);          // Front Sharp IR Sensor
  pinMode(A1, INPUT);          // Rear Sharp IR Sensor
  pinMode(A2, INPUT);          // Left Wheel Encoder
  pinMode(A3, INPUT);          // Right Wheel Encoder
  pinMode(A6, INPUT);          // Left Sharp IR Sensor
  pinMode(A7, INPUT);          // Right Sharp IR Sensor
  
  
  digitalWrite(A0, HIGH);       // Enable pullup resistors so pins do not float without a sensor attached.
  digitalWrite(A1, HIGH); 
  digitalWrite(A2, HIGH); 
  digitalWrite(A3, HIGH); 
  digitalWrite(A6, HIGH); 
  digitalWrite(A7, HIGH); 
  

  // Set up the I2C Slave address

     Wire.begin(SLAVE_ADDRESS); 

     Wire.onRequest(requestEvent);

     Wire.onReceive(receiveEvent);
     
     registerMap[0x0F] = IDENTIFICATION; // ID register
     registerMap[0x0F] = IDENTIFICATION; // ID register


  ReportTime < millis();
  
  Serial.begin (57600);
  Serial.println("RST, Running Encoder_06. 131122");

  
  sei();
 
} 

void loop(){


  lenc = analogRead(A2);
  
  if(lstate == 0 && lenc > threshold) {
    lstate = 1; lpos++;
  } else if(lstate == 1 && lenc < threshold) {
    lstate = 0; lpos++;  
  }  

  
  renc = analogRead(A3);

  if(rstate == 0 && renc > threshold) {
    rstate = 1; rpos++; 
  } else if(rstate == 1 && renc < threshold) {
    rstate = 0; rpos++;
  }  
  
  if(ReportTime < millis()){    // Dont send dumps any more frequent than once per second
      ReportTime = millis()+1000;

        Front_New =  analogRead(A0);  Front_Avg = Front_Tally / SMOOTH; Front_Tally += (Front_New-Front_Avg);
        Rear_New =  analogRead(A1);   Rear_Avg = Rear_Tally / SMOOTH;   Rear_Tally += (Rear_New-Rear_Avg);
        Left_New  =  analogRead(A6);  Left_Avg = Left_Tally / SMOOTH;   Left_Tally += (Left_New-Left_Avg);
        Right_New  =  analogRead(A7); Right_Avg = Right_Tally / SMOOTH; Right_Tally += (Right_New-Right_Avg);
  
  
       
      Serial.print("  Analog pin A2 : ");
      Serial.print(lenc);
      Serial.print("  LPOS "); Serial.print(lpos);  
    
      Serial.print("  Analog pin A3 : ");
      Serial.print(renc);
      Serial.print("  RPOS   "); Serial.println(rpos);

      Serial.print("Front: ");  Serial.print(Front_Avg); Serial.print("  Rear: ");  Serial.print(Rear_Avg);  Serial.print("  Left: ");  Serial.print(Left_Avg); Serial.print("  Right: ");  Serial.println(Right_Avg);

      for (int i = 0; i < (REG_MAP_SIZE - 1); i++) {
         Serial.print(registerMap[i],HEX); Serial.print(" ");
       }   
      Serial.println();

  }


    if(zeroD || zeroE)           //  Is I2C master updating Config or mode registers?
    {
          newDataAvailable = 0;  //  we don’t have any post correction data available
          changeModeConfig();    //  call the function to make your changes
          return;                //  go back to the beginning and start collecting data from scratch

    }

    storeData();
    newDataAvailable = 1;            // Let the I2C Master know that data is available.

}



void receiveEvent(int bytesReceived)
{
  Serial.print(".");
     for (int a = 0; a < bytesReceived; a++)
     {
          if ( a < MAX_SENT_BYTES)
          {
               receivedCommands[a] = Wire.read();
          } else
          {
               Wire.read();  // if we receive more data then allowed just throw it away
          }
     }
     if(bytesReceived == 1 && (receivedCommands[0] < REG_MAP_SIZE))  return;   // Setting up for next command

     if(bytesReceived == 1 && (receivedCommands[0] >= REG_MAP_SIZE))           // Error condition...
     {
          receivedCommands[0] = 0x00;
          return;
     }

    switch(receivedCommands[0]){

          case 0x0D:
               zeroD = 1;                          //status flag to let us know we have new data in register 0x0D
               zeroDData = receivedCommands[1];    //save the data to a separate variable
               bytesReceived--;
               if(bytesReceived == 1)  return;     //only two bytes total were sent so we’re done
 
               zeroE = 1;
               zeroEData = receivedCommands[2];
               return;              //we simply return here because the most bytes we can receive is three anyway
               break;

          case 0x0E:
               zeroE = 1;
               zeroEData = receivedCommands[1];
               return;             //we simply return here because 0x0E is the last writable register
               break;

          default:
               return;             // ignore the commands and return
     }
}

 
void requestEvent()
{
     Wire.write(registerMap+receivedCommands[0], REG_MAP_SIZE); 
     lpos = 0; rpos = 0;
}

void storeData()
{

     registerMap[0] = Status;  
     registerMap[1] = lowByte(lpos);
     registerMap[2] = highByte(lpos);
     registerMap[3] = lowByte(rpos);
     registerMap[4] = highByte(rpos);
     registerMap[5] = lowByte(Front_Avg);
     registerMap[6] = highByte(Front_Avg);
     registerMap[7] = lowByte(Rear_Avg);
     registerMap[8] = highByte(Rear_Avg);
     registerMap[9] = lowByte(Left_Avg);
     registerMap[10] = highByte(Left_Avg);
     registerMap[11] = lowByte(Right_Avg);
     registerMap[12] = highByte(Right_Avg);

     registerMap[13] = modeRegister;
     registerMap[14] = configRegister;

}

 void changeModeConfig()
{
     /*Put your code here to evaluate which of the registers need changing
          And how to make the changes to the given device.  For our GPS example
          It could be issuing the commands to change the baud rate, update rate,
          Datum, etc… */

     modeRegister = zeroDData;
     configRegister = zeroEData;
     zeroD = 0;
     zeroE = 0;   //always make sure to reset the flags before returning

}
