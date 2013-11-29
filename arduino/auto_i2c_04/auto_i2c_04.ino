

// Motion Commands
#define STOP 0
#define WAIT 1
#define SCAN 2
#define LOOK_LEFT 3
#define LOOK_RIGHT 4
#define LOOK_AHEAD 5
#define LOOK_TOWARD 6
#define LOOK_UP 7

#define MOVE_FORWARD 10
#define MOVE_REVERSE 11
#define TURN_LEFT 12
#define TURN_RIGHT 13
#define SPIN_LEFT 14
#define SPIN_RIGHT 15
#define TURN_TOWARD 16
#define IN_MOTION 17
#define SET_SPEED 18

#define DUMP_SENSORS 20
#define DUMP_SOUNDINGS 21
#define READ_HEADING 22
#define READ_ACCEL 23
#define READ_DISTANCE 24
#define SCAN_I2C 25

//Pin 0   Input  RX 
//Pin 1   Output TX 
//PIN 2   Input  MaxSonar Front
//PIN 3   Input  MaxSonar Rear
//Pin 4   UNUSED
//Pin 5   UNUSED 
//Pin 6   RHT_03 Temperature and Humidity Sensor
//Pin 7   UNUSED
//PIN 8   UNUSED
//PIN 9   Output Sensor Pod Pan Servo
//PIN 10  Output Sensor Pod Tilt Servo 
//PIN 11  UNUSED
//PIN 12  UNUSED
//PIN 13  UNUSED
//PIN A5  I2C SDA
//PIN A4  I2C SCL  
//PIN A3  UNUSED
//PIN A2  UNUSED 
//PIN A1  LIPO Battery Voltage
//PIN A0  UNUSED


#include <string.h>
#include <ctype.h>


#include <Wire.h>                // Include the Wire library so we can start using I2C.



#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Create the motor shield object with the default I2C address
Adafruit_DCMotor *RightMotor = AFMS.getMotor(1);
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(2);



// Include the Love Electronics ADXL345 library so we can use the accelerometer.
#include <ADXL345.h>
ADXL345 accel;                        // Declare a global instance of the accelerometer.
AccelerometerScaled AccScaled;
float xAxisGs = 0;


int LAMPS = A2;
int FrntPIN = 2;                      // MaxSonar Front Sensor
int RearPIN = 3;                      // MaxSonar Rear Sensor

int CPUVoltsPin = A1;
float CPUVolts = 0;
#define FullBattery 18;                // A Full Battery is 18v

int HMC6352SlaveAddress = 0x42;        // Compass unit
int HMC6352ReadAddress = 0x41;         //"A" in hex, A command is: 


// Following are related to the Wheel Encoder and Proximity sensor board
#define  Encoder_Board   0x29              // I2C slave address of Wheel encoder and IR sensors 
#define  REG_MAP_SIZE    16
#define  MAX_SENT_BYTES  3
byte i2cData[16];
unsigned int lpos = 0, rpos = 0, front = 0,rear = 0,left = 0,right = 0;
int encstat;                              // Encoder Status

unsigned int LTravelled_Ticks = 0, RTravelled_Ticks = 0;
float Millimeters_Travelled = 0;

char send_string[127];                    // Formatted string for sending information to Raspberry PI

float CurrentHeading = 0;
float PreviousHeading = 0;
float temperature = 0, humidity=0;

int SOC = 0;          
int Command = 0;
int Parameter = 0;
int UID = 0;
int Scanpos = 0;       // Last Scan Position
unsigned long ScanTime; // Time when the most recent scan started.  (Index to keep scans grouped)
unsigned long ReportTime; // Time when the most recent sensor dump was sent.  

//variables to store motion states

int motion=STOP, spd=50, already_stopped=0;             // Cuurent state of motion :  HALT, MOVE_FORWARD, MOVE_REVERSE, TURN_LEFT, TURN_RIGHT
unsigned long MotionStart, MotionStop, now;              // Holders for the millis() dependant start and stop timers.

//variables needed to store values
int fpulse, rpulse, DistanceAhead, DirectAhead, DistanceBehind, DirectBehind, DirectRight, DirectLeft;
int ppos=90;            // Starting position of Sensor pod pan.... looking straight ahead.
int tpos=90;            // Starting position of Sensor pod tilt.... looking straight ahead.


#include <DHT22.h>
// Data wire is plugged into port 6 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
#define DHT22_PIN 6            // RHT03  Temperature and Humidity


// Setup a DHT22 instance
DHT22 myDHT22(DHT22_PIN);


#include <Servo.h>

Servo panservo;  // create servo object to control POD panning
Servo tiltservo;  // create servo object to control POD tilt 
 

void setup()
{
  
  HMC6352SlaveAddress = HMC6352SlaveAddress >> 1; // I know 0x42 is less than 127, but this is still required

  panservo.attach(9);                // attaches the servo on pin 9 to the servo object 
  tiltservo.attach(10);              // attaches the servo on pin 10 to the servo object 
  tiltservo.write(90);               // tell servo to go to position 90 (straight up)
  
  panservo.write(180);               // tell servo to go to position 180 to get left  and right soundings
  new_delay(5);
  
  Get_Distance();
  DirectLeft=DistanceAhead;         // Get a handle on how close we are to objects left and right.
  DirectRight=DistanceBehind; 
 
  panservo.write(ppos);              // tell servo to go to position in variable 'pos'
  new_delay(5);
  Get_Distance();
  DirectAhead=DistanceAhead; 
  DirectBehind=DistanceBehind; 

  ReportTime = millis();
  
  Serial.begin(57600);
  Serial.println("RST, Running Auto_I2C_04. 131129");
  
  // Start the I2C Wire library so we can use I2C to talk to the Accelerometer and Compass.
  Wire.begin();
  

  accel = ADXL345();                 // Create an instance of the accelerometer on the default address (0x1D)
  accel.SetRange(2, true);           // Set the range of the accelerometer to a maximum of 2G.
  accel.EnableMeasurements();        // Tell the accelerometer to start taking measurements.
  
  AFMS.begin();  // create with the default frequency 1.6KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  RightMotor->setSpeed(spd);
  LeftMotor->setSpeed(spd);
  
  RightMotor->run(FORWARD);
  LeftMotor->run(FORWARD);
  // turn on motor
  RightMotor->run(RELEASE);
  LeftMotor->run(RELEASE);

  pinMode(LAMPS, OUTPUT);  //Initiates lapms pin
  analogWrite(LAMPS, 255);    //Turn OFF lamps


  randomSeed(analogRead(0));
 
}
 
 
void loop()
{
 
  now++;
  if (Millimeters_Travelled > MotionStop && motion != STOP) {
      RightMotor->run(RELEASE);
      LeftMotor->run(RELEASE);
      sprintf(send_string, "MVST,%d,%d", MotionStop, UID);
      Serial.println(send_string);
      motion = STOP;
      LTravelled_Ticks = 0; RTravelled_Ticks = 0;
  }    
  
  evade();                // If anything is within the Evade_Threshold  get out of the way. 
    
  read_sensors();
  
  // if there's any serial available, read it:
  if (Serial.available()) {
      SOC= Serial.parseInt();        // Start of Command  sequence . Must be "99" 
      Command= Serial.parseInt();
      Parameter=Serial.parseInt();
      UID=Serial.parseInt();
   
    if (SOC == 99) {                // Validate that command starts with "99"
      sprintf(send_string, "CMD,%d,%d,%d,%d", Command,Parameter,UID,now);
      Serial.println(send_string);

      switch (Command) {
        case STOP:
           stp();
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
        
        case SCAN:
           Scan_Local(Parameter);
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
   
        case LOOK_TOWARD:        
           ppos =constrain(Parameter, 0, 180);  // Servo can only take 0-180
           panservo.write(ppos);
           new_delay(7);
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
   
        case LOOK_AHEAD:      
           ppos=90;                          // Center sensor pod.
           panservo.write(ppos);
           new_delay(7);
           Get_Distance();
           DirectAhead=DistanceAhead;
           DirectBehind=DistanceBehind;
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
       
        case LOOK_RIGHT:
           ppos =0;
           panservo.write(ppos);
           new_delay(7);
           Get_Distance();
           DirectRight=DistanceAhead;     // Get a handle on how close we are to objects left and right.
           DirectLeft=DistanceBehind; 
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
       
        case LOOK_LEFT:
           ppos =180;
           panservo.write(ppos);
           new_delay(7);
           Get_Distance();
           DirectLeft=DistanceAhead;     // Get a handle on how close we are to objects left and right.
           DirectRight=DistanceBehind; 
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
       
        case LOOK_UP:
           constrain(Parameter, 0, 90); 
           tpos = Parameter + 90;            // At rest, the pod is at 90... 
           tiltservo.write(tpos);
           new_delay(7);
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
        case DUMP_SENSORS:
           Send_Sensors();
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
       
       case MOVE_FORWARD:
           motion=MOVE_FORWARD;
           MotionStart=Millimeters_Travelled;
           MotionStop=MotionStart+ Parameter;                                  
           move_forward();      
        break;
       
        case MOVE_REVERSE:
           motion=Command;
           MotionStart=Millimeters_Travelled;
           MotionStop=MotionStart+ Parameter;                                  
           move_reverse();
        break;
       
        case TURN_LEFT:
           motion=Command;
           MotionStart=Millimeters_Travelled;
           MotionStop=MotionStart+ Parameter;                                  
           turn_left();
        break;
       
        case TURN_RIGHT:
           motion=Command;
           MotionStart=Millimeters_Travelled;
           MotionStop=MotionStart+ Parameter;                                  
           turn_right();
        break;
       
        case TURN_TOWARD:
           turn_to(Parameter);
        break;

       case SET_SPEED:
           spd = constrain(Parameter, 0, 250);  // Speed is a single byte: 0 is full STOP: 255 is full GO
           Serial.print("RDY,");
           Serial.println(UID);
        break;
      
        default:
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
             
      }                      // Close switch statement
   } else {                  // Close command validation 'if'
      if (UID > 0) {

        sprintf(send_string, "FAIL,%d,%d,%d,%d", Command,Parameter,UID,now);
        Serial.println(send_string);
      }
   }     
  }                          // Close serial port content validation
}                            // Close 'loop'
  

void read_sensors()
{
      // Read the *scaled* data from the accelerometer 
      // This useful method gives you the value in G thanks to the Love Electronics library.
      AccScaled = accel.ReadScaledAxis();
      PreviousHeading = CurrentHeading;
      CurrentHeading=Get_Compass();
      Get_Distance();
      if (ppos == 90) {
         DirectAhead=DistanceAhead;          // Get distance directly ahead and behind for obstacle avoidance
         DirectBehind=DistanceBehind;
      }
      CPUVolts = 0.017578 * analogRead(CPUVoltsPin);        // 18v Full Battery divided by 1024 is 0.017578
      
      if (now % 1000) {   // RHT03 Sensor can only be read about every 2 seconds
          temperature = myDHT22.getTemperatureC();
          humidity = myDHT22.getHumidity();
      }    
      
      read_proximity();            // Get Wheel Encoders and Sharp IR sensors

    if (ReportTime++ > 2){    // Dont send dumps any more frequent than once per second
      ReportTime = 0;
      Send_Sensors();
    }  

}  
  


void Scan_Local(int incr) {


  if (!incr)  incr=1;
  ScanTime = millis();          // Time the most recent scan started
  for(ppos=90; ppos > 0; ppos -= incr)         // goes from 0 degrees to 180 degrees 
  {                                       // in steps of 5 degree
     panservo.write(ppos);                  // tell servo to go to position in variable 'pos'
     new_delay(5);       // waits 15ms for the servo to reach the position
     Get_Distance();
     sprintf(send_string, "SCAN,%d,%d,%d,%d,%d,%d", ScanTime,90-ppos,DistanceAhead,270-ppos,DistanceBehind,UID);
     Serial.println(send_string);

  }
  Get_Distance();
  DirectLeft=DistanceAhead;          // Get distance directly ahead and behind for obstacle avoidance
  DirectRight=DistanceBehind;
  Send_Sensors();
  panservo.write(180);              // tell servo to go to position in variable 'pos'
  new_delay(10);                     // waits 15ms for the servo to reach the position
  for(ppos=180; ppos> 90; ppos -= incr)       // Return to Center position
  {                                
      panservo.write(ppos);              // tell servo to go to position in variable 'pos'
      new_delay(5);       // waits 15ms for the servo to reach the position
     Get_Distance();
     sprintf(send_string, "SCAN,%d,%d,%d,%d,%d,%d", ScanTime,180-ppos,DistanceBehind,360-ppos,DistanceAhead,UID);
     Serial.println(send_string);
 }
  Get_Distance();
  DirectAhead=DistanceAhead;          // Get distance directly ahead and behind for obstacle avoidance
  DirectBehind=DistanceBehind;
}


float Get_Compass()
{
      //Time to read the Compass.   
    //"Get Data. Compensate and Calculate New Heading"
    Wire.beginTransmission(HMC6352SlaveAddress);
    Wire.write(HMC6352ReadAddress);              // The "Get Data" command
    Wire.endTransmission();

  //time delays required by HMC6352 upon receipt of the command
  //Get Data. Compensate and Calculate New Heading : 6ms
  new_delay(6);

  Wire.requestFrom(HMC6352SlaveAddress, 2); //get the two data bytes, MSB and LSB

  //"The heading output data will be the value in tenths of degrees
  //from zero to 3599 and provided in binary format over the two bytes."
  byte MSB = Wire.read();
  byte LSB = Wire.read();

  float HeadingSum = (MSB << 8) + LSB; //(MSB / LSB sum)
  float HeadingInt = HeadingSum / 10; 
  return HeadingInt;
}


void Get_Distance()
{
    pinMode(FrntPIN, INPUT);
    pinMode(RearPIN, INPUT);

    //Used to read in the pulse that is being sent by the MaxSonar device.
    //Pulse Width representation with a scale factor of 147 uS per Inch.
    // We are getting 5 samples and averaging the results.
    
    fpulse=0; rpulse=0;
    for(int i=0;i<4;i+=1)
    {  
       new_delay(10);
       fpulse += (pulseIn(FrntPIN, HIGH)/147);       //147uS per inch
       new_delay(10);
       rpulse += (pulseIn(RearPIN, HIGH)/147);       //147uS per inch
    }
    DistanceAhead = fpulse/5;
    DistanceBehind = rpulse/5;
   
}




// Output the data down the serial port.
//  Sensors: accel x,y,z, compass heading, Distance ahead,Behind, Temperature, Humidity, Battery\n

void Send_Sensors() {
   // Tell us about the this data, but scale it into useful units (G).
   Serial.print("SNSR,");  
   Serial.print(now);                Serial.print(",");  
   Serial.print(motion);             Serial.print(",");  
   Serial.print(AccScaled.XAxis);    Serial.print(",");   
   Serial.print(AccScaled.YAxis);    Serial.print(",");   
   Serial.print(AccScaled.ZAxis);    Serial.print(",");
   Serial.print(CurrentHeading);     Serial.print(",");
   Serial.print(ppos);               Serial.print(",");
   Serial.print(tpos);               Serial.print(",");
   Serial.print(DirectAhead);        Serial.print(",");
   Serial.print(DirectBehind);       Serial.print(",");
   Serial.print(DirectLeft);         Serial.print(",");
   Serial.print(DirectRight);        Serial.print(",");
   Serial.print(rpos);               Serial.print(",");
   Serial.print(lpos);               Serial.print(",");
   Serial.print(temperature);        Serial.print(",");
   Serial.print(humidity);           Serial.print(",");
   Serial.print(CPUVolts);           Serial.print(",");
   Serial.println(UID);              
   
   //Serial.print("Millimeters_Travelled   ");    Serial.println(Millimeters_Travelled);      
   //Serial.print("LTravelled_Ticks   "); Serial.print(LTravelled_Ticks);  Serial.print("   RTravelled_Ticks   ");    Serial.print(RTravelled_Ticks);      
   
  
  
}


/* **************************************************************************************************************************************
*  Motion Commands Here.
************************************************************************************************************************************** */

void stp()
{  
  RightMotor->run(RELEASE);
  LeftMotor->run(RELEASE);
  MotionStop = Millimeters_Travelled;
  motion = STOP;
  LTravelled_Ticks = 0; RTravelled_Ticks = 0;
}


void move_forward()
{
   RightMotor->setSpeed(spd);
   LeftMotor->setSpeed(spd);
  
   RightMotor->run(FORWARD);
   LeftMotor->run(FORWARD);   
}

void move_reverse()
{
   RightMotor->setSpeed(spd);
   LeftMotor->setSpeed(spd);
  
   RightMotor->run(BACKWARD);
   LeftMotor->run(BACKWARD);
}

void turn_left()
{
   RightMotor->setSpeed(spd);
   LeftMotor->setSpeed(spd);
  
   RightMotor->run(FORWARD);
   LeftMotor->run(BACKWARD);
} 

void turn_right()
{
   RightMotor->setSpeed(spd);
   LeftMotor->setSpeed(spd);
  
   RightMotor->run(BACKWARD);
   LeftMotor->run(FORWARD);
} 


float turn_to(float NewHeading)
{
  float CurrentHeading = Get_Compass();
  spd = 60;   // Take it slowly to allow Compass to track.
  if (CurrentHeading < NewHeading)
  {
     while (CurrentHeading < NewHeading)      // Check for CurrentHeading if it's 
     {                                        // higher than the NewHeading, rotate 
       motion=TURN_RIGHT;
       MotionStart=Millimeters_Travelled;
       MotionStop=MotionStart+ 10;                                  
       turn_right();
       CurrentHeading = Get_Compass();        // ****************************************
     } return CurrentHeading;                 // Need to add "determine closest direction"
  } else while (CurrentHeading > NewHeading)
     {
       motion=TURN_LEFT;
       MotionStart=Millimeters_Travelled;
       MotionStop=MotionStart+ 10;                                  
       turn_left();
       CurrentHeading = Get_Compass();
     } return CurrentHeading; 
        
    
}

void evade()
{
 
  if(DirectAhead < 6) {          // Obect in front is closer than 8 inches
    if(DirectBehind > 6) {        // Make sure we have room to back up
      motion=MOVE_REVERSE;
      MotionStart=Millimeters_Travelled; MotionStop=MotionStart+100; // Back up a few inches 
      move_reverse();
    }  else {
         if(DirectLeft > 12 && DirectLeft > DirectRight) {  // *********** Determine best turn right or left *****************
             Serial.print("MVEL,"); Serial.print(", "); Serial.println(UID);
             motion=TURN_LEFT;
             MotionStart=Millimeters_Travelled; MotionStop=MotionStart+100; // Back up a few inches 
             turn_left();
         } else if (DirectRight > 12) {
             Serial.print("MVER,"); Serial.print(", "); Serial.println(UID);
             motion=TURN_RIGHT;
             MotionStart=Millimeters_Travelled; MotionStop=MotionStart+100; // Back up a few inches 
             turn_left();
         }   
    }      
    
    
  } else if(DirectBehind < 6) {          // Obect behind is closer than 6 inches
    Serial.print("MVEF,"); Serial.print(", "); Serial.println(UID);
    if(DirectAhead > 10) {        // Make sure we have room to go forward
      motion=MOVE_FORWARD;
      MotionStart=Millimeters_Travelled; MotionStop=MotionStart+100; // Move forward a few inches 
      move_forward();
    }  else {
         if(DirectLeft > 12 && DirectLeft > DirectRight) {  // *********** Determine best turn right or left *****************
             Serial.print("MVEL,"); Serial.print(", "); Serial.println(UID);
             motion=TURN_LEFT;
             MotionStart=Millimeters_Travelled; MotionStop=MotionStart+100; // Back up a few inches 
             turn_left();
         } else if (DirectRight > 12) {
             Serial.print("MVER,"); Serial.print(", "); Serial.println(UID);
             motion=TURN_RIGHT;
             MotionStart=Millimeters_Travelled; MotionStop=MotionStart+100; // Back up a few inches 
             turn_right();
         }   
    }                            
  }                               
}


void read_proximity()            // This retrieves and smooths (averages) the data received from the Sharp IR sensors
{                               //  It also retrieves the Wheel encoder ticks.
 int index = 0; 
  
  Wire.requestFrom(Encoder_Board, 16);    // request 16 bytes from encoder board
  while(Wire.available())    // slave may send less than requested
  { 
    
    char c = Wire.read(); // receive a byte as character
    i2cData[index] = c;
    //Serial.print(i2cData[index], HEX);   Serial.print(" ");      // print the character
    index++;
  }
  //Serial.println(); 
  encstat = i2cData[0];
  rpos = (i2cData[2] << 8) | i2cData[1];      // rebuild integers from I2C bytes word(lowbyte,highbyte)
  lpos = (i2cData[4] << 8) | i2cData[3];
  front = (i2cData[6] << 8) | i2cData[5];
  rear = (i2cData[8] << 8) | i2cData[7];
  left = (i2cData[10] << 8) | i2cData[9];
  right = (i2cData[12] << 8) | i2cData[11];
  
  LTravelled_Ticks +=lpos; RTravelled_Ticks += rpos;
  Millimeters_Travelled = (LTravelled_Ticks + RTravelled_Ticks)/2 * 2.49;    
  //  Average of left and right * 2.49mm, the distance travelled in an encoder transition
}



void new_delay(unsigned long int d) {                // Replace the delay function to remove dependancy on timer0
   for (unsigned long int a=0;a<d;a++){
     for(int b=0;b<3174;b++){
       __asm__("nop\n\t");       // Each nop command takes about 62ns at 16mhz
     }  
   }
}
