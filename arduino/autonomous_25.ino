

// Motion Commands
#define STOP 0
#define WAIT 1
#define SCAN 2
#define LOOK_LEFT 3
#define LOOK_RIGHT 4
#define LOOK_AHEAD 5
#define LOOK_TOWARD 6

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

//Pin 0   Input  RX 
//Pin 1   Output TX 
//PIN 2   Input  MaxSonar Front
//PIN 3   Output PWM Motor A 
//Pin 4   Output Sensor Pod Servo
//Pin 5   Input  
//Pin 6   UNUSED
//Pin 7   Input  MaxSonar Rear
//PIN 8   Output Brake Motor B
//PIN 9   Output Brake Motor A
//PIN 10  Output 
//PIN 11  Output PWM Motor B
//PIN 12  Output DIR Motor A
//PIN 13  Output DIR Motor B
//PIN A5  Input  Voltage Sense Control Battery
//PIN A4  UNUSED  
//PIN A3  UNUSED
//PIN A2  UNUSED 
//PIN A1  UNUSED
//PIN A0  UNUSED


#include <string.h>
#include <ctype.h>

// Include the Wire library so we can start using I2C.
#include <Wire.h>
// Include the Love Electronics ADXL345 library so we can use the accelerometer.
#include <ADXL345.h>

// Declare a global instance of the accelerometer.
ADXL345 accel;

int CPUVoltsPin = A0;
int LAMPS = A1;
int FrntPIN = 2;
int RearPIN = 7;

int CPUVolts = 0;

int HMC6352SlaveAddress = 0x42;
int HMC6352ReadAddress = 0x41; //"A" in hex, A command is: 
byte i2cData[16];

AccelerometerScaled AccScaled;
float CurrentHeading = 0;
float PreviousHeading = 0;
float xAxisGs = 0;
float anVolt=0,anVoltR=0;

int SOC = 0;          
int Command = 0;
int Parameter = 0;
int UID = 0;
int ScanPos = 0;       // Last Scan Position
unsigned long ScanTime; // Time when the most recent scan started.  (Index to keep scans grouped)
unsigned long ReportTime; // Time when the most recent sensor dump was sent.  

//variables to store motion states

int motion=STOP, spd=100, already_stopped=0;             // Cuurent state of motion :  HALT, MOVE_FORWARD, MOVE_REVERSE, TURN_LEFT, TURN_RIGHT
unsigned long MotionStart, MotionStop, now;              // Holders for the millis() dependant start and stop timers.

//variables needed to store values
int fpulse, rpulse, DistanceAhead, DirectAhead, DistanceBehind, DirectBehind, DirectRight, DirectLeft;
int pos=90;            // Starting position of Sensor pod.... looking straight ahead.
#include <DHT22.h>

// Data wire is plugged into port 6 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
#define DHT22_PIN 6

// Setup a DHT22 instance
DHT22 myDHT22(DHT22_PIN);


#include <Servo.h>

Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created
 

void setup()
{
  
  HMC6352SlaveAddress = HMC6352SlaveAddress >> 1; // I know 0x42 is less than 127, but this is still required

  myservo.attach(4);              // attaches the servo on pin 4 to the servo object
  myservo.write(180);              // tell servo to go to position 0
  delay(5);
  Get_Distance();
  DirectLeft=DistanceAhead;     // Get a handle on how close we are to objects left and right.
  DirectRight=DistanceBehind; 
 
  myservo.write(pos);              // tell servo to go to position in variable 'pos'
  delay(5);
  Get_Distance();
  DirectAhead=DistanceAhead; 
  DirectBehind=DistanceBehind; 

  ReportTime = millis();
  
  Serial.begin(57600);
  Serial.println("RST, Running Autonomous_25. 130927");
  // Start the I2C Wire library so we can use I2C to talk to the Accelerometer and Compass.

  Wire.begin();
  
   // Create an instance of the accelerometer on the default address (0x1D)
  accel = ADXL345();
  
  // Set the range of the accelerometer to a maximum of 2G.
  accel.SetRange(2, true);
  // Tell the accelerometer to start taking measurements.
  accel.EnableMeasurements();
  
   //Setup Motor Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin
  pinMode(9, OUTPUT); //Initiates Brake Channel A pin

  //Setup Motor Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel A pin
  pinMode(8, OUTPUT);  //Initiates Brake Channel A pin

  pinMode(A1, OUTPUT);  //Initiates Brake Channel A pin
  analogWrite(LAMPS, 255);    //Turn OFF lamps


  randomSeed(analogRead(0));
 
}
 
 
void loop()
{
 
  now = millis();
  Motion();
     
  Read_Sensors();
  
  // if there's any serial available, read it:
  if (Serial.available()) {
      SOC= Serial.parseInt();        // Start of Command  sequence . Must be "99" 
      Command= Serial.parseInt();
      Parameter=Serial.parseInt();
      UID=Serial.parseInt();
   
    if (SOC == 99) {                // Validate that command starts with "99"
      Serial.print("CMD, ");
      Serial.print(Command);
      Serial.print(", ");
      Serial.print(Parameter);
      Serial.print(", ");
      Serial.print(UID);
      Serial.print(", ");
      Serial.println(now);

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
           pos = constrain(Parameter, 0, 180);  // Servo can only take 0-180
           Look_To(pos);
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
   
        case LOOK_AHEAD:      
           pos=90;                          // Center sensor pod.
           Look_To(pos);
           Get_Distance();
           DirectAhead=DistanceAhead;
           DirectBehind=DistanceBehind;
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
       
        case LOOK_RIGHT:
           pos = 0;
           Look_To(pos);
           Get_Distance();
           DirectRight=DistanceAhead;     // Get a handle on how close we are to objects left and right.
           DirectLeft=DistanceBehind; 
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
       
        case LOOK_LEFT:
           pos = 180;
           Look_To(pos);
           Get_Distance();
           DirectLeft=DistanceAhead;     // Get a handle on how close we are to objects left and right.
           DirectRight=DistanceBehind; 
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
           MotionStart=millis();
           MotionStop=MotionStart+Parameter;
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
       
        case MOVE_REVERSE:
           motion=Command;
           MotionStart=millis();
           MotionStop=MotionStart+Parameter;
           Serial.print("RDY, ");
           Serial.println(UID);
        break;
       
        case TURN_LEFT:
           motion=Command;
           MotionStart=millis();
           MotionStop=MotionStart+Parameter;
           Serial.print("RDY,");
           Serial.println(UID);
        break;
       
        case TURN_RIGHT:
           motion=Command;
           MotionStart=millis();
           MotionStop=MotionStart+Parameter;
           Serial.print("RDY,");
           Serial.println(UID);
        break;
       
        case TURN_TOWARD:
           Turn_To(Parameter);
           Serial.print("RDY,");
           Serial.println(UID);
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
        Serial.print("FAIL, ");
        Serial.print(Command);
        Serial.print(", ");
        Serial.print(Parameter);
        Serial.print(", ");
        Serial.print(UID);
        Serial.print(", ");
        Serial.println(now);
      }
   }     
  }                          // Close serial port content validation
}                            // Close 'loop'
  

void Read_Sensors()
{
      // Read the *scaled* data from the accelerometer 
      // This useful method gives you the value in G thanks to the Love Electronics library.
      AccScaled = accel.ReadScaledAxis();
      PreviousHeading = CurrentHeading;
      CurrentHeading=Get_Compass();
      delay(10);     // waits 15ms for the servo to reach the position
      Get_Distance();
      if (pos == 90) {
         DirectAhead=DistanceAhead;          // Get distance directly ahead and behind for obstacle avoidance
         DirectBehind=DistanceBehind;
      }
      CPUVolts = analogRead(CPUVoltsPin);
    if (ReportTime < millis()){    // Dont send dumps any more frequent than once per second
      ReportTime = millis()+1000;
      Send_Sensors();
    }  
}  
  


void Scan_Local(int incr) {


  if (!incr)  incr=1;
  ScanTime = millis();          // Time the most recent scan started
  for(pos=90; pos > 0; pos -= incr)         // goes from 0 degrees to 180 degrees 
  {                                       // in steps of 5 degree
     myservo.write(pos);                  // tell servo to go to position in variable 'pos'
     delay(5);       // waits 15ms for the servo to reach the position
     Get_Distance();
     Serial.print("SCAN,"); Serial.print(ScanTime); Serial.print(", "); 
     Serial.print(90-pos);  Serial.print(", "); Serial.print(DistanceAhead); Serial.print(", "); 
     Serial.print(270-pos);  Serial.print(", "); Serial.println(DistanceBehind); 


  }
  Get_Distance();
  DirectLeft=DistanceAhead;          // Get distance directly ahead and behind for obstacle avoidance
  DirectRight=DistanceBehind;
  Send_Sensors();
  myservo.write(180);              // tell servo to go to position in variable 'pos'
  delay(10);                     // waits 15ms for the servo to reach the position
  for(pos = 180; pos> 90; pos -= incr)       // Return to Center position
  {                                
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(5);       // waits 15ms for the servo to reach the position
     Get_Distance();
     Serial.print("SCAN,"); Serial.print(ScanTime);  Serial.print(", ");
     Serial.print(270-pos);  Serial.print(", ");Serial.print(DistanceBehind);  Serial.print(", "); 
     Serial.print(360-pos);  Serial.print(", ");Serial.println(DistanceAhead); 
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
  delay(6);

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
       delay(10);
       fpulse += (pulseIn(FrntPIN, HIGH)/147);       //147uS per inch
       delay(10);
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
   Serial.print( now);
   Serial.print(",");   
   Serial.print(motion);
   Serial.print(",");  
   
   Serial.print(AccScaled.XAxis);
   Serial.print(",");   
   Serial.print(AccScaled.YAxis);
   Serial.print(",");   
   Serial.print(AccScaled.ZAxis);
   Serial.print(",");
   Serial.print(CurrentHeading);
   Serial.print(",");
   Serial.print(pos);
   Serial.print(",");
   Serial.print(DirectAhead);
   Serial.print(",");
  Serial.print(DirectBehind);
  Serial.print(",");
   Serial.print(DirectLeft);
   Serial.print(",");
  Serial.print(DirectRight);
  Serial.print(",");
  Serial.print(i2cData[1]);
   Serial.print(",");
  Serial.print(i2cData[2]);
  Serial.print(",");
  Serial.print(CPUVolts);
  Serial.print(",");
  Serial.print(UID);
  Serial.println();
  
}



void Motion()
{
  
  if (MotionStop < millis()) { 
    if (motion != STOP) {
      analogWrite(3, 0);    //Spins the motor on Channel A at speed 0
      analogWrite(11, 0);   //Spins the motor on Channel B at speed 0
      motion = STOP;
    } 
  } 
  //evade();                 // Check surroundings for threats or obstacles
  switch(motion)
    {
      case STOP:
       if(already_stopped != 0) {
         Serial.print("MVST, "); Serial.print(MotionStop); Serial.print(", "); Serial.println(UID);
         analogWrite(3, 0);    //Spins the motor on Channel A at speed 0
         analogWrite(11, 0);   //Spins the motor on Channel B at speed 0
         motion = STOP;
         already_stopped = 0;
       }  
      break;
      case IN_MOTION:
        already_stopped = 1;
      break;
      case MOVE_FORWARD:
         Serial.print("MVFW, "); Serial.print(MotionStart); Serial.print(", "); Serial.print(MotionStop); Serial.print(", "); Serial.println(UID);
         motion = IN_MOTION;        
            //Motor A dir
             digitalWrite(12, 0);  //Establishes direction of Channel A
             digitalWrite(9, LOW);   //Disengage the Brake for Channel A
             analogWrite(3, spd);    //Spins the motor on Channel A at speed
  
             //Motor B dir
             digitalWrite(13, 0); //Establishes direction of Channel B
             digitalWrite(8, LOW);   //Disengage the Brake for Channel B
             analogWrite(11, spd);   //Spins the motor on Channel B at speed
          
      break;
      case MOVE_REVERSE:
        Serial.print("MVRV, "); Serial.print(MotionStart); Serial.print(", "); Serial.print(MotionStop); Serial.print(", "); Serial.println(UID);

         motion = IN_MOTION;        
             //Motor A dir
             digitalWrite(12, 1);  //Establishes direction of Channel A
             digitalWrite(9, LOW);   //Disengage the Brake for Channel A
             analogWrite(3, spd);    //Spins the motor on Channel A at speed
  
             //Motor B dir
             digitalWrite(13, 1); //Establishes direction of Channel B
             digitalWrite(8, LOW);   //Disengage the Brake for Channel B
             analogWrite(11, spd);   //Spins the motor on Channel B at speed
          
      break;
      case  TURN_LEFT:
        Serial.print("MVTL, "); Serial.print(MotionStart); Serial.print(", "); Serial.print(MotionStop); Serial.print(", "); Serial.println(UID);
         motion = IN_MOTION;        
             //Motor A dir
             digitalWrite(12, 0);  //Establishes direction of Channel A
             digitalWrite(9, LOW);   //Disengage the Brake for Channel A
             analogWrite(3, spd);    //Spins the motor on Channel A at speed
  
             //Motor B dir
             digitalWrite(13, 1); //Establishes direction of Channel B
             digitalWrite(8, LOW);   //Disengage the Brake for Channel B
             analogWrite(11, spd);   //Spins the motor on Channel B at speed
          
      break;
      case  TURN_RIGHT:
         Serial.print("MVTR, "); Serial.print(MotionStart); Serial.print(", "); Serial.print(MotionStop); Serial.print(", "); Serial.println(UID);
         motion = IN_MOTION;        
             //Motor A dir
             digitalWrite(12, 1);  //Establishes direction of Channel A
             digitalWrite(9, LOW);   //Disengage the Brake for Channel A
             analogWrite(3, spd);    //Spins the motor on Channel A at speed
  
             //Motor B dir
             digitalWrite(13, 0); //Establishes direction of Channel B
             digitalWrite(8, LOW);   //Disengage the Brake for Channel B
             analogWrite(11, spd);   //Spins the motor on Channel B at speed
          
      break;
    }  
}


void Look_To(int pos)
{
    delay(5);       // waits 15ms for the servo to reach the position
    myservo.write(pos);                  // tell servo to go to position in variable 'pos'
    delay(5);       // waits 15ms for the servo to reach the position

}

float Turn_To(float NewHeading)
{
  float CurrentHeading = Get_Compass();
  Serial.print("MVTT, "); Serial.print(MotionStart); Serial.print(", "); Serial.print(MotionStop);  Serial.print(", "); Serial.println(UID);
  
  if (CurrentHeading < NewHeading)
  {
     while (CurrentHeading < NewHeading)      // Check for CurrentHeading if it's 
     {                                        // higher than the NewHeading, rotate 
       motion=TURN_RIGHT;
       MotionStart=millis();
       MotionStop=MotionStart+Parameter;
       Motion();
       CurrentHeading = Get_Compass();        // ****************************************
     } return CurrentHeading;                 // Need to add "determine closest direction"
  } else while (CurrentHeading > NewHeading)
     {
       motion=TURN_LEFT;
       MotionStart=millis();
       MotionStop=MotionStart+Parameter;
       Motion();
       CurrentHeading = Get_Compass();
     } return CurrentHeading; 
        
    
}

void evade()
{
 
  if(DirectAhead < 6) {          // Obect in front is closer than 8 inches
    Serial.print("MVEB,"); Serial.print(", "); Serial.println(UID);
    if(DirectBehind > 6) {        // Make sure we have room to back up
      motion=MOVE_REVERSE;
      MotionStart=millis(); MotionStop=MotionStart+600; // Back up a few inches 
    }  else {
         if(DirectLeft > 12 && DirectLeft > DirectRight) {  // *********** Determine best turn right or left *****************
             Serial.print("MVEL,"); Serial.print(", "); Serial.println(UID);
             motion=TURN_LEFT;
             MotionStart=millis(); MotionStop=MotionStart+600; // Back up a few inches 
         } else if (DirectRight > 12) {
             Serial.print("MVER,"); Serial.print(", "); Serial.println(UID);
             motion=TURN_RIGHT;
             MotionStart=millis(); MotionStop=MotionStart+600; // Back up a few inches 
         }   
    }      
    
    
  } else if(DirectBehind < 6) {          // Obect behind is closer than 6 inches
    Serial.print("MVEF,"); Serial.print(", "); Serial.println(UID);
    if(DirectAhead > 10) {        // Make sure we have room to go forward
      motion=MOVE_FORWARD;
      MotionStart=millis(); MotionStop=MotionStart+600; // Move forward a few inches 
    }  else {
         if(DirectLeft > 12 && DirectLeft > DirectRight) {  // *********** Determine best turn right or left *****************
             Serial.print("MVEL,"); Serial.print(", "); Serial.println(UID);
             motion=TURN_LEFT;
             MotionStart=millis(); MotionStop=MotionStart+600; // Back up a few inches 
         } else if (DirectRight > 12) {
             Serial.print("MVER,"); Serial.print(", "); Serial.println(UID);
             motion=TURN_RIGHT;
             MotionStart=millis(); MotionStop=MotionStart+600; // Back up a few inches 
         }   
    }                            
  }                               
}


void stp()
{  
  digitalWrite(9, HIGH);  //Engage the Brake for Channel A
  analogWrite(3, 0);    //Spins the motor on Channel A at speed
 
  digitalWrite(8, HIGH);  //Engage the Brake for Channel B
  analogWrite(11, 0);   //Spins the motor on Channel B at speed
  MotionStop = millis();
  Serial.print("MVST, "); Serial.print(MotionStop); Serial.print(", "); Serial.println(UID);
  motion = STOP;
}




