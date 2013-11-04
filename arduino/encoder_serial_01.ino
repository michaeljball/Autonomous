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
#define SDAPIN      A4
#define SCLPIN      A5
#define Left_IRPin  A6
#define Right_IRPin A7



#define FORWARD 1
#define REVERSE 0
#define WHITE 1
#define BLACK 0
#define LEFT 65
#define RIGHT 66
#define THRESHOLD 700             // 700 seemed like a reasonable set point for transition.  Play with this...

#define LAST_ADC_INPUT 15
#define FIRST_ADC_INPUT 0
#define   ADC_VREF_VCC   0x40
#define   ADC_VREF_AREF  0x00
#define   ADC_VREF_BG    0x80
#define   ADC_VREF_INT   0xc0


#define ADC_VREF_TYPE ADC_VREF_VCC 

volatile unsigned int LencoderPos = 0;
volatile unsigned int RencoderPos = 0;

volatile unsigned int adc_value[16];
volatile unsigned int adc_raw = 0;
static unsigned char input_index = 0;

int Left_Encoder = BLACK;
int Right_Encoder = BLACK;
unsigned long now;
int dir = FORWARD;


// High when a value is ready to be read
volatile int requestFlag=0;

// Value to store analog result
volatile int LanalogVal, RanalogVal, Front_IRVal, Rear_IRVal, Left_IRVal, Right_IRVal;


#include <Wire.h>

#define  SLAVE_ADDRESS   0x29              // I2C slave address 
#define  REG_MAP_SIZE    16
#define  MAX_SENT_BYTES  3

#define  INTERRUPT_PIN           2
#define  IDENTIFICATION           0x0F

byte registerMap[REG_MAP_SIZE];
byte registerMapTemp[REG_MAP_SIZE - 1];
byte receivedCommands[MAX_SENT_BYTES];    
/* three bytes is the most amount of data that would ever need to be sent from the master.  
The first byte sent from the master is always the register address.  
For our device, we only have two writable registers in our map and they are the configuration 
register and mode register.  The rest of the registers are read only so they can’t be written to.
*/


byte newDataAvailable = 0;
byte Status = 0;
byte useInterrupt = 0;
byte modeRegister = B0000001;
byte configRegister = B00000000;
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

  //  With thanks to http://www.glennsweeney.com/tutorials/interrupt-driven-analog-conversion-with-an-atmega328p
  //  for all of the ADC interrupt code and concepts
  // And thanks to http://www.dsscircuits.com/articles/arduino-i2c-slave-guide.html 
  // for the I2C Slave mode implementation.
  
  
  // clear ADLAR in ADMUX (0x7C) to right-adjust the result
  // ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits)
  ADMUX &= B11011111;
 
  // Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
  // proper source (01)
  ADMUX |= B01000000;
 
  // Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog
  // input
  ADMUX &= B11110000;
 
  // Set MUX3..0 in ADMUX (0x7C) to read from AD8 (Internal temp)
  // Do not set above 15! You will overrun other parts of ADMUX. A full
  // list of possible inputs is available in Table 24-4 of the ATMega328
  // datasheet
  ADMUX |= 1;
  // ADMUX |= B00000010; // Binary equivalent
 
  // Set ADEN in ADCSRA (0x7A) to enable the ADC.
  // Note, this instruction takes 12 ADC clocks to execute
  ADCSRA |= B10000000;
 
  // Set ADATE in ADCSRA (0x7A) to enable auto-triggering.
  ADCSRA |= B00100000;
 
  // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
  // This means that as soon as an ADC has finished, the next will be
  // immediately started.
  ADCSRB &= B11111000;
 
  // Set the Prescaler to 128 (16000KHz/128 = 125KHz)
  // Above 200KHz 10-bit results are not reliable.
  ADCSRA |= B00000111;
 
  // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
  // Without this, the internal interrupt will not trigger.
  ADCSRA |= B00001000;
 
  // Enable global interrupts
  // AVR macro included in <avr/interrupts.h>, which the Arduino IDE
  // supplies by default.
  sei();
 
  // Kick off the first ADC
  // Set ADSC in ADCSRA (0x7A) to start the ADC conversion
  ADCSRA |=B01000000;

 if(useInterrupt)

  {

    pinMode(INTERRUPT_PIN,OUTPUT);
    digitalWrite(INTERRUPT_PIN,HIGH);

  }

  
  Serial.begin (57600);
  Serial.println("RST, Running Encoder_Serial_01. 131103");

} 

void loop(){
   cli();                          // Disable interrupts momentarily so that the values are not corrupted. 
   Front_IRVal = adc_value[0];      //  Use local copy of Analog values for processing
   Rear_IRVal =  adc_value[1];

   LanalogVal = adc_value[2];      //  Use local copy of Analog values for processing
   RanalogVal = adc_value[3];

   Left_IRVal = adc_value[6];      //  Use local copy of Analog values for processing
   Right_IRVal =  adc_value[7];
   sei();                          //  Re-enable interrupts.


   // Set up LEFT and RIGHT toggle counters for the wheel encoders Black and White stripes.
   // On a 3" Wheel
    // Check to see if the value has been updated
  
    // Test for encoder wheel transitions left and right depending on ADMUX value
    if(Left_Encoder == BLACK && LanalogVal<THRESHOLD) {
      Left_Encoder = WHITE;
      //Serial.println("Updating LEFT white");    // Comment out when working
      upd_counter(LEFT); 
    }  
    else if(Left_Encoder == WHITE && LanalogVal>THRESHOLD) {
      Left_Encoder = BLACK;
      //Serial.println("Updating LEFT black");
      upd_counter(LEFT); 
    }    


    if(Right_Encoder == BLACK && RanalogVal<THRESHOLD) {
      Right_Encoder = WHITE;
      //Serial.println("Updating RIGHT white");
      upd_counter(RIGHT); 
    }  
    else if(Right_Encoder == WHITE && RanalogVal>THRESHOLD) {
      Right_Encoder = BLACK;
      //Serial.println("Updating RIGHT black");
      upd_counter(RIGHT); 
    }

     if (newDataAvailable == 1) { 
       Serial.print("Data Avaliable: FRNT  ");
       Serial.print(Front_IRVal);
       Serial.print(", REAR  ");
       Serial.print(Rear_IRVal);
       Serial.print(",  LEFT ");
        Serial.print(Left_IRVal);      
       Serial.print(",  RIGHT ");
       Serial.print(Right_IRVal);      
       Serial.print(",  LENCVAL ");
      Serial.print(LanalogVal);
       Serial.print(",   RENCVAL ");
       Serial.print(RanalogVal);
       Serial.print(",   LENCPOS ");
       Serial.print(LencoderPos);      
       Serial.print(",   RENCPOS ");
       Serial.println(RencoderPos);      
    

       
       requestFlag = 0;
     }  

    now = millis();
   
    storeData();
    newDataAvailable = 1;            // Let the I2C Master know that data is available.
    toggleInterrupt();
}

void upd_counter(int side)
{
      if (dir == FORWARD ) {    //  Record transition to Black
        if (side == LEFT) LencoderPos++;
        else RencoderPos++;
      } 
      else {
        if (side == LEFT) LencoderPos--;
        else RencoderPos--;
      }
      storeData();
      newDataAvailable = 1;
}


//
// **************************************************************************
// *
// *      A D C _ I S R
// *
// **************************************************************************
//
// ADC interrupt service routine
ISR(ADC_vect) 
{

// Read the AD conversion result
   adc_raw = ADCW;

// Do whatever needs to be done for averaging.  Let's try a 2 sample average
   adc_value[input_index] += adc_raw;
   adc_value[input_index] /= 2;

// *** SINCE YOU WANT TO FINISH A "ROUND" YOU'LL JUST STORE

// Select next ADC input
   if (++input_index > (LAST_ADC_INPUT-FIRST_ADC_INPUT))
   {
      input_index=0;       // THAT IS FOR CONTINUOUS CONVERSIONS.  YOU'D SET A FLAG, AND >>NOT<< START THE NEXT CONVERSION.
   }

   ADMUX=(FIRST_ADC_INPUT|ADC_VREF_TYPE)+input_index;


   ADCSRA|=0x40;    // Start the AD conversion
} 



void storeData()
{

     byte * bytePointer;  // we declare a pointer as type byte
     byte arrayIndex = 1; // we need to keep track of where we are storing data in the array

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

    Front_IRVal = adc_value[0];      //  Use local copy of Analog values for processing
   Rear_IRVal =  adc_value[1];

   LanalogVal = adc_value[2];      //  Use local copy of Analog values for processing
   RanalogVal = adc_value[3];

   Left_IRVal = adc_value[6];      //  Use local copy of Analog values for processing
   Right_IRVal =  adc_value[7];



     registerMapTemp[0] = newDataAvailable;  //no need to use a pointer for gpsStatus
     bytePointer = (byte*)&LencoderPos; //LencoderPos is 2 bytes
     for (int i = 1; i > -1; i--)
     {
          registerMapTemp[arrayIndex] = bytePointer[i];  //increment pointer to store each byte
          arrayIndex++;
     }

     bytePointer = (byte*)&RencoderPos; //RencoderPos is 2 bytes
     for (int i = 1; i > -1; i--)
     {
          registerMapTemp[arrayIndex] = bytePointer[i];  //increment pointer to store each byte
          arrayIndex++;
     }

     bytePointer = (byte*)&Front_IRVal; //Front_IRVal is 2 bytes
     for (int i = 1; i > -1; i--)
     {
          registerMapTemp[arrayIndex] = bytePointer[i];  //increment pointer to store each byte
          arrayIndex++;
     }

     bytePointer = (byte*)&Rear_IRVal; //Rear_IRVal is 2 bytes
     for (int i = 1; i > -1; i--)
     {
          registerMapTemp[arrayIndex] = bytePointer[i];  //increment pointer to store each byte
          arrayIndex++;
     }
     bytePointer = (byte*)&Left_IRVal; //Left_IRVal is 2 bytes
     for (int i = 1; i > -1; i--)
     {
          registerMapTemp[arrayIndex] = bytePointer[i];  //increment pointer to store each byte
          arrayIndex++;
     }
     bytePointer = (byte*)&Right_IRVal; //Right_IRVal is 2 bytes
     for (int i = 1; i > -1; i--)
     {
          registerMapTemp[arrayIndex] = bytePointer[i];  //increment pointer to store each byte
          arrayIndex++;
     }


     registerMapTemp[arrayIndex] = modeRegister;

     arrayIndex++;

     registerMapTemp[arrayIndex] = configRegister;

     arrayIndex++;

     registerMapTemp[arrayIndex] = IDENTIFICATION;

}


 void toggleInterrupt()

{

  if(!useInterrupt)

  {return;} //first let’s make sure we’re using interrupts, if not just return from the function

  if(newDataAvailable)  // if new data is available set the interrupt low

  {

    digitalWrite(INTERRUPT_PIN,LOW);  //set pin low and return

    return;

  }

  //no new data available or data was just read so set interrupt pin high

  digitalWrite(INTERRUPT_PIN,HIGH); 

}


