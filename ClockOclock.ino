
// ***** Timer *****
#include <SimpleTimer.h>
SimpleTimer timer; // the timer object
// timer library - https://github.com/zomerfeld/SimpleTimerArduino

// ***** PID *****
#include <PID_v1.h>

// ***** RTC *****
#include <RTClib.h> //library from https://github.com/adafruit/RTClib
RTC_DS1307 rtc;
DateTime now;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int nowHour = -1;
int nowMinute = -1;
int nowSecond = -1;
int lastHour = -1;
int lastMinute = -1;
int lastSecond = -1;

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
int magnetHigh = 520; // high range for magnet detection (460+578?)
int magnetLow = 480;

// home-test parametrs - REMOVEEEEEEE
//int magnetHigh = 700; // high range for magnet detection (460+578?)
//int magnetLow = 100;

// *** SERIAL VARIABLES ***
String readString; //This while store the user input data
int User_Input = 0; // This while convert input string into integer
boolean stringComplete = false;  // whether the string is complete

// *** CLOCK VARIABLES ***
bool motorDisabled = 0; //To disable the motor passed limits
long maxPosition = 25L * 40000L;
//long cmdPosition = 200; // Where we're aiming the motor to go
bool motionDone = 1; // If the clock's in motion or not
long distanceMinute = 18; // CHANGE - How much we need to move for one minute passing - in angles
long distance5Second = 5; // CHANGE - How much we need to move for 5 seconds passing - in angles

// *** ENCODER VARIABLES ***
int encoderPin1 = 2; //Encoder Output 'A' must connected with intreput pin of arduino.
int encoderPin2 = 3; //Encoder Otput 'B' must connected with intreput pin of arduino.
//         Best Performance: both pins have interrupt capability
//         Good Performance: only the first pin has interrupt capability
//         Low Performance:  neither pin has interrupt capability
//         Make sure to pull UP those pins up with a 1K resistor (though the code seems to use the INPUT_PULLUP)
volatile int lastEncoded = 0; // Here updated value of encoder store.
volatile long encoderValue = 0; // Raw encoder value
int PPR = 7124;  // Encoder Pulse per revolution.
int angle = 180; // Maximum degree of motion.
int REV = 0;          // Set point REQUIRED ENCODER VALUE
int lastMSB = 0;
int lastLSB = 0;

int maxGap = 20; // how much tolerance for overshoot or undershoot to say "good enough" and STOP motor. 20 seems a good value.

//these are  storage containers for messaging
long oldPosition  = -999;
long newPosition;

// ************************** SETUP **************************
void setup() {

  // ***** STARTS SERIAL & RTC *****
  Serial.begin(250000);
  Serial.println("Rachel's Clock");
  Serial.println("***STARTING RTC***");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
  readString.reserve(200);   // reserve 200 bytes for the readString


  // Setting Timers
  timer.setInterval(4999, showTime); // This will display the time every 5 seconds on serial. Disable if needed.
  //  timer.setInterval(1000, minuteMove); // This will move the motor every minute. Not needed currently
  //  timer.setInterval(5000, fiveSecMove); //moves the motor every 5 second forward. Should not be enabled by default


  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time,
  // rtc.adjust(DateTime(2017, 6, 2, 15, 29, 0));


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

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  // *** call updateEncoder() when any high/low changed seen
  //     on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);


  TCCR1B = TCCR1B & 0b11111000 | 1;  // set 31KHz PWM to prevent motor noise

  // *** Setting PID ***
  myPID.SetMode(AUTOMATIC);   //set PID in Auto mode
  myPID.SetSampleTime(1);  // refresh rate of PID controller
//  myPID.SetOutputLimits(-200, 200); // this is the MAX PWM value to move motor, here change in value reflect change in speed of motor.
  motorSpeed(200); // Sets Motor Speed // 70 is too slow 

  digitalWrite(enablePin, HIGH); // Turns the motor on

  debouncer1.attach(fwdButton);
  debouncer2.attach(backButton);
  debouncer1.interval(90); // 90 seemed to work fast enough. Test and modify if needed
  debouncer2.interval(90); // 90 seemed to work fast enough. Test and modify if needed

  // If using Digital Limit Switch - , setup the Bounce instance (not needed for magnetic limit sw) :
  //  debouncer.attach(limitSwPin);
  //  debouncer.interval(90); // 90 seemed to work fast enough. Test and modify if needed

  findEdges();

}

// ************************** LOOP **************************

void loop() {
  // ***** UPDATE TIME *****
  timer.run();

  // ***** READ ENCODER *****
  newPosition = encoderValue;
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.print ("encoder position: "); // Prints value changes. DEBUG - Disable eventually
    Serial.println(newPosition); // DEBUG - Disable eventually
  }


  debouncer1.update(); //checks the switches
  debouncer2.update();

  // *** Debug LED for Limit Switches
  if ((analogRead(limitSwPin) >= magnetHigh) || (analogRead(limitSwPin) <= magnetLow)) { // numbers might need adjusting based on analog reads of hall sensor
    digitalWrite(debugLED, HIGH); //turn on debug led
  } else {
    digitalWrite(debugLED, LOW); //turn off debug LED
  }

  // **** Serial handling  ****
  if (stringComplete) {
    Serial.println(User_Input);
    Serial.println(readString.toInt());  //printing the input data in integer form
    User_Input = readString.toInt();   // here input data is store in integer form
    REV = map (User_Input, 0, angle, 0, PPR); // mapping degree into pulse
    setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value

  }

  input = encoderValue ;           // data from encoder consider as a Process value

// *** CHECK FOR STOP ***
  double gap = abs(setpoint - input); //distance away from setpoint
  if (gap < maxGap) { //we're close to setpoint, stop
    stopMotor();
  }
  else // **** MOVE MOVE MOVE ****
  {
    //we're far from setpoint
    myPID.Compute();                 // calculate new output
    pwmOut(output);
    Serial.print("this is REV: ");
    Serial.println(REV);
    Serial.print("encoderValue: ");
    Serial.print(encoderValue);
    Serial.print("   (g:");
    Serial.print(gap);
    Serial.println(")");

  }

  // clear the string: (should I put this into stopmotor?)
  readString = ""; // Cleaning User input, ready for new Input
  stringComplete = false;


  // ***** MANUAL CONTROL BUTTONS *****
  if (debouncer1.fell()) { //if the button went to low (set to pullup)
    analogWrite(motorSpeedPin, 200);
    forward();
    Serial.println("moving manually");
  }

  if (debouncer2.fell()) {
    analogWrite(motorSpeedPin, 200);
    reverse();
    Serial.println("moving manually");
  }

  if (debouncer1.rose()) { //when the buttons are not pressed anymore
    Serial.println("Button 1 rose");
    stopMotor();
  }

  if (debouncer2.rose()) {
    Serial.println("Button 2 rose");
    stopMotor();
  }

}


void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  lastEncoded = encoded; //store this value for next time

}
