
// ***** Timer *****
unsigned long previousMillis = 0;        // will store last time LED was updated
const long printInterval = 5000;           // interval at which to print (milliseconds)


// ***** PID *****
#include <PID_v1.h>

// ***** RTC *****
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
//#include "RTClib.h"
//RTC_DS1307 rtc;
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;


// RTC Wiring: 5V to 5V, GND to GND, SCL to A5 (on Uno, changes by controller), SDA to A4 (on Uno)
// TC Wiring: https://screencast.com/t/50Cv0fAUM7w5

#include <Bounce2.h>
// Initiate a Bounce object: //needed for digital switch
Bounce debouncer1 = Bounce();
Bounce debouncer2 = Bounce();


// *** PID VALUES ***
double kp = 10 , ki = 14 , kd =  0.001;             // modify for optimal performance
double input = 0, output = 0, setpoint = 0;
PID myPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);

// *** MOTOR CONTROL PINS ***
#define enablePin 4 // Connect to EN - Send High or Low to enabled and disable the motor
#define motorSpeedPin 5 // Connect to D2 on the Pololu MD05A (Needs to be PWM pin)
#define CWPin 6 // Connect to IN1 - Turning HIGH will change the motor direction to ClockWise
#define CCWPin 7 // Connect to IN2 - Turning HIGH will change the motor direction to Counter ClockWise
// (flip the A/B wires if the direction is reversed)

#define fwdButton 8 //Button to move motor forward
#define backButton 9 //Button to move motor backward
#define debugLED 13 // Debug LED


// *** LIMIT SWITCH  ***
#define limitSwPin A0 // The pin for the limit switch.  
// For a regular switch, set as INPUT_PULLUP and connect the switch to GND and look for LOW for trigger. (https://www.arduino.cc/en/Tutorial/DigitalInputPullup)
int magnetHigh = 530; // It will only move if between these values!
int magnetLow = 499;


// *** SERIAL VARIABLES ***
String readString; //This while store the user input data
int User_Input = 0; // This while convert input string into integer
boolean stringComplete = false;  // whether the string is complete

// *** CLOCK VARIABLES ***
bool motorDisabled = 0; //To disable the motor passed limits
long maxPosition = 25L * 40000L;
long minPosition = 25L * 40000L;


//long cmdPosition = 200; // Where we're aiming the motor to go
bool motionDone = 1; // If the clock's in motion or not
long distanceMinute = 18; // CHANGE - How much we need to move for one minute passing - in angles
long distance5Second = 5; // CHANGE - How much we need to move for 5 seconds passing - in angles

// *** ENCODER VARIABLES ***
int encoderPin1 = 3; //Encoder Output 'A' must connected with intreput pin of arduino.
int encoderPin2 = 2; //Encoder Otput 'B' must connected with intreput pin of arduino.
//         Best Performance: both pins have interrupt capability
//         Good Performance: only the first pin has interrupt capability
//         Low Performance:  neither pin has interrupt capability
//         Make sure to pull UP those pins up with a 1K resistor (though the code seems to use the INPUT_PULLUP)
volatile int lastEncoded = 0; // Here updated value of encoder store.
volatile long encoderValue = 0; // Raw encoder value
int PPR = 7124;  // Encoder Pulse per revolution.
int minAngle = 12; // degrees to map with magents
int maxAngle = 170; // degrees to map with magnets
int destAngle = 0;
int REV = 0;          // Set point REQUIRED ENCODER VALUE
int lastMSB = 0;
int lastLSB = 0;

int maxGap = 20; // how much tolerance for overshoot or undershoot to say "good enough" and STOP motor. 20 seems a good value.

//these are  storage containers for messaging
long oldPosition  = -999;
long newPosition;


// *** ALARM / SECOND VARIABLES ***
// make this edit to TimeAlarms.h
// #define dtNBR_ALARMS 24  // max is 255
AlarmId id;
bool incrementalToggle = false;

int secondSpeed = 95; //the speed to move when moving once per second
int secondAngle = 1; // the angle-per-move change when moving once a second


// ************************************************************************************************************************************************







// ************************** SETUP **************************

void setup() {

  // ***** STARTS SERIAL & RTC *****
  Serial.begin(250000);
  Serial.println("Rachel's Clock");
  Serial.println("***STARTING RTC***");
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  setSyncInterval(8000);         // set the number of seconds between re-sync
  readString.reserve(200);   // reserve 200 bytes for the readString

  // *** Set time on RTC ***
  // These lines sets the RTC with an explicit date & time,
//  setTime(23,58,20,9,25,2023); //setTime(hr,min,sec,day,month,yr);
//  RTC.set(now());
  bool parse=false;
  bool config=false;
//
//  // get the date and time the compiler was run
//  if (getDate(__DATE__) && getTime(__TIME__)) {
//    parse = true;
//    // and configure the RTC with this info
//    if (RTC.write(tm)) {
//      config = true;
//    }
//  }


  // ***** PIN SETUP *****
  pinMode(enablePin, OUTPUT);
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(CWPin, OUTPUT);
  pinMode(CCWPin, OUTPUT);
  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  pinMode(limitSwPin, INPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(fwdButton, INPUT_PULLUP);
  pinMode(backButton, INPUT_PULLUP);
  pinMode(debugLED, OUTPUT);

  // encoder parameters
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
  // *** call updateEncoder() when any high/low changed seen
  //     on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  REV = map (90, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value      REV = map (User_Input, 10, angle, minPosition, PPR); // mapping degree into pulse

  TCCR1B = TCCR1B & 0b11111000 | 1;  // set 31KHz PWM to prevent motor noise

  // *** Setting PID ***
  myPID.SetMode(AUTOMATIC);   //set PID in Auto mode
  myPID.SetSampleTime(1);  // refresh rate of PID controller
  //  myPID.SetOutputLimits(-200, 200); // this is the MAX PWM value to move motor, here change in value reflect change in speed of motor.
  motorSpeed(200); // Sets Motor Speed // 70 is too slow

  // Buttons
  debouncer1.attach(fwdButton);
  debouncer2.attach(backButton);
  debouncer1.interval(90); // 90 seemed to work fast enough. Test and modify if needed
  debouncer2.interval(90); // 90 seemed to work fast enough. Test and modify if needed

  // If using Digital Limit Switch - , setup the Bounce instance (not needed for magnetic limit sw) :
  //  debouncer.attach(limitSwPin);
  //  debouncer.interval(90); // 90 seemed to work fast enough. Test and modify if needed

  // motor settings
  digitalWrite(enablePin, HIGH); // Turns the motor on
  findEdges(); // **** Start the find edges module ****


  // ***** Test Alarms *****
  //  Alarm.alarmRepeat(23,17,0, move90fm);
  //  Alarm.alarmRepeat(23,18,0, move100fm);
  //  Alarm.timerRepeat(5, showTime); // show time every 5 seconds

  //  moveSecAl = Alarm.timerRepeat(6, moveSec); // move every 6 seconds -- disabled currently

  // ****** FM ALARMS ******
  Alarm.alarmRepeat(20, 59, 58, move90fm) ;//should be (20, 59, 58, move90fm);
  Alarm.alarmRepeat(22, 00, 01, move100fm); //should be (22, 00, 01, move100fm)
  Alarm.alarmRepeat(23, 00, 00, moveRest);  //should be (23, 00, 00, moveRest)
  //
  // ****** AM ALARMS ******
  Alarm.alarmRepeat(5, 49, 58, move550am); // Should Be Alarm.alarmRepeat(5, 49, 58, move550am);
  Alarm.alarmRepeat(6, 0 , 0, move600am); // Should Be Alarm.alarmRepeat(6, 0, 0, move600am);
  Alarm.alarmRepeat(7, 0, 0, move700am); // Should Be Alarm.alarmRepeat(7, 0, 0, move700am);
  Alarm.alarmRepeat(8, 0, 0, move800am);  // Should Be Alarm.alarmRepeat(8, 0, 0, move800am);
  Alarm.alarmRepeat(9, 0, 0, move900am);// Should Be Alarm.alarmRepeat(9, 0, 0, move900am);
  Alarm.alarmRepeat(10, 0, 0, move1000am); // Should Be Alarm.alarmRepeat(10, 0, 0, move1000am);
  Alarm.alarmRepeat(11, 0, 0, move1100am); // Should Be Alarm.alarmRepeat(11, 0, 0, move1100am);
  Alarm.alarmRepeat(12, 0, 0, moveRest); // Should Be Alarm.alarmRepeat(12, 0, 0, moveRest);

  // NOT READY THERE MIGHT BE A MAX NUMBER OF ALARMS AND I HAVE TO FIGURE THAT OUT AND RELEASE SOME

}


// **********************************************************





// ************************** LOOP **************************

void loop() {
  // ***** UPDATE TIME *****
  unsigned long currentMillis = millis(); // all time elements are data type unsigned long
  Alarm.delay(0); // Needed to make the time library work.

  // *** Print Time ***
  if ((currentMillis - previousMillis) >= printInterval) { // enough time passed yet?
    previousMillis = currentMillis;
    showTime();
  }

  //  if (incrementalToggle == false) { // decide if to move incrementally (every second) or not
  //    Alarm.disable(moveSecAl);
  //  } else {
  //    Alarm.enable(moveSecAl);
  //  }


  // ***** READ ENCODER *****
  newPosition = encoderValue;
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    if ((currentMillis - previousMillis) >= printInterval) { // enough time passed yet?
      previousMillis = currentMillis;
      Serial.print ("encoder position: "); // Prints value changes. DEBUG - Disable eventually
      Serial.println(newPosition); // DEBUG - Disable eventually
    }
  }

  //checks the switches
  debouncer1.update(); //checks the switches
  debouncer2.update();

  // ***** MANUAL CONTROL BUTTONS *****

  if (debouncer1.fell()) { //if the button went to low (set to pullup)
    digitalWrite(debugLED, HIGH); //turn on debug led
    Serial.println("moving manually");
    forward();
    analogWrite(motorSpeedPin, 200);
  }

  if (debouncer2.fell()) {
    digitalWrite(debugLED, HIGH); //turn on debug led
    Serial.println("moving manually");
    reverse();
    analogWrite(motorSpeedPin, 200);
  }

  if (debouncer1.rose()) { //when the buttons are not pressed anymore
    Serial.println("Button 1 rose");
    digitalWrite(debugLED, LOW); //turn off debug led
    setpoint = encoderValue;
    stopMotor();
  }

  if (debouncer2.rose()) {
    Serial.println("Button 2 rose");
    digitalWrite(debugLED, LOW); //turn off debug led
    setpoint = encoderValue;
    stopMotor();
  }

  // ********************

  // *** Debug LED for Limit Switches
  if ((analogRead(limitSwPin) >= magnetHigh) || (analogRead(limitSwPin) <= magnetLow)) { // numbers might need adjusting based on analog reads of hall sensor
    digitalWrite(debugLED, HIGH); //turn on debug led
    if ((currentMillis - previousMillis) >= printInterval) { // enough time passed yet?
      previousMillis = currentMillis;
      Serial.print("LIMIT - HALL SENSOR READING: ");
      Serial.println(analogRead(limitSwPin));
    }
  } else {
    digitalWrite(debugLED, LOW); //turn off debug LED
  }

  // **** Serial handling  ****
  if (stringComplete) {
    readString.trim();
    Serial.println(readString);
    if (readString == "b") {
      Serial.println("moving back a bit");
      reverse();
      analogWrite(motorSpeedPin, 150);
      Alarm.delay(250);
      setpoint = encoderValue;
      stopMotor();
      Serial.println("stopped manual move");

    } else if (readString == "f") {
      Serial.println("moving fwd a bit");
      forward();
      analogWrite(motorSpeedPin, 150);
      Alarm.delay(250);
      setpoint = encoderValue;
      stopMotor();
      Serial.println("stopped manual move");
    } else if (readString == "reset") {
      Serial.println("Reset - FindingEdges");
      findEdges();
      clrSrString();
    } else if (readString == "time") {
      showTime();
    } else if (readString == "m90f") {
      Serial.println("moving to 90FM");
      move90fm ();
      clrSrString();
    } else if (readString == "m100f") {
      Serial.println("moving to 100fm");
      move100fm();
      clrSrString();
    } else if (readString == "m550a") {
      move550am ();
      clrSrString();
    } else if (readString == "m600a") {
      move600am ();
      clrSrString();
    } else if (readString == "m700a") {
      move700am ();
      clrSrString();
    } else if (readString == "m800a") {
      move800am ();
      clrSrString();
    } else if (readString == "m900a") {
      move900am ();
      clrSrString();
    } else if (readString == "m1000a") {
      move1000am ();
      clrSrString();
    } else if (readString == "m1100a") {
      move1100am ();
      clrSrString();
    } else if (readString == "rest") {
      moveRest ();
      clrSrString();


    } else {
      Serial.println(readString.toInt());  //printing the input data in integer form
      User_Input = readString.toInt();   // here input data is store in integer form
      if (User_Input < 5) {
        User_Input = 5;
      }
      REV = map (User_Input, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
      setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value

    }
  }

  input = encoderValue ;           // data from encoder consider as a Process value


  // *** CHECK FOR DESTINATION STOP *** //DOES THIS WORK???? (NZ)
  double gap = abs(setpoint - input); //distance away from setpoint
  if ((digitalRead(fwdButton) == 1) && (digitalRead(backButton) == 1)) {
    if (gap < maxGap) { //we're close to setpoint, stop
      //      if ((currentMillis - previousMillis) >= printInterval) { // enough time passed yet?
      //        previousMillis = currentMillis;
      //        Serial.println("*** CLOSE TO GAP ***"); // DEBUG - Disable eventually
      //        Serial.print("Buttons State ");
      //        Serial.print(digitalRead(fwdButton));
      //        Serial.print("       ");
      //        Serial.println(digitalRead(backButton));
      //    }
      stopMotor();
    }
    else // **** MOVE MOVE MOVE ****
    {
      //we're far from setpoint
      myPID.Compute();                 // calculate new output
      pwmOut(output);
      //      Serial.print("this is REV: ");
      //      Serial.println(REV);
      Serial.print("encoderValue: ");
      Serial.print(encoderValue);
      Serial.print("   (g:");
      Serial.print(gap);
      Serial.println(")");

    }
  }

  // clear the string:
  clrSrString();


  //  // Print the hall sensor reading (Every time? NZ)
  //  if ((currentMillis - previousMillis) >= printInterval) { // enough time passed yet?
  //    previousMillis = currentMillis;
  //    Serial.print("LIMIT - HALL SENSOR READING: ");
  //    Serial.println(analogRead(limitSwPin));
  //  }


} //  *** end of loop ***


//encoder function
void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time

}
