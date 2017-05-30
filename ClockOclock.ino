/* Encoder Library - Basic Example
   http://www.pjrc.com/teensy/td_libs_Encoder.html
*/

// If you define ENCODER_DO_NOT_USE_INTERRUPTS *before* including
// Encoder, the library will never use interrupts.  This is mainly
// useful to reduce the size of the library when you are using it
// with pins that do not support interrupts.  Without interrupts,
// your program must call the read() function rapidly, or risk
// missing changes in position.
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>
#include <Bounce2.h>


//   Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
//   Make sure to pull those pins up with a 1K resistor (though the code seems to use the INPUT_PULLUP

Encoder myEnc(2, 3); //on Uno, the pins with interrupt capability are 2 and 3 (https://www.arduino.cc/en/Reference/attachInterrupt)

// *** MOTOR CONTROL PINS ***
#define enablePin 4 // Connect to EN - Send High or Low to enabled and disable the motor
#define motorSpeedPin 5 // Connect to D2 on the Pololu MD05A (Needs to be PWM pin)
#define CWPin 6 // Connect to IN1 - Turning HIGH will change the motor direction to ClockWise
#define CCWPin 7 // Connect to IN2 - Turning HIGH will change the motor direction to Counter ClockWise
// (flip the A/B wires if the direction is reversed)
// **************************

// *** LIMIT SWITCH  ***
#define limitSwPin 8 // The pin for the limit switch. Set to pullup, so defaults to high. 
// Connect the switch to GND and look for LOW for trigger. (https://www.arduino.cc/en/Tutorial/DigitalInputPullup)
// Instantiate a Bounce object :
Bounce debouncer = Bounce(); 
// **************************

// *** SERIAL COMMANDS VARIABLES ***
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
// **************************

// *** CLOCK VARIABLES ***
long cmdPosition = 0; // Where we're aiming the motor to go
bool motionDone = 1; // If the clock's in motion or not
// **************************

// *** ENCODER VARIABLES ***
long oldPosition  = -999;
// **************************


void setup() {
  // ***** STARTS SERIAL *****
  Serial.begin(250000);
  inputString.reserve(200);   // reserve 200 bytes for the inputString
  Serial.println("Rachel's Clock");

  // ***** SETS PINS *****
  pinMode(enablePin, OUTPUT);
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(CWPin, OUTPUT);
  pinMode(CCWPin, OUTPUT);
  pinMode(limitSwPin, INPUT_PULLUP);
  pinMode(enablePin, OUTPUT);
  // After setting up the button, setup the Bounce instance :
  debouncer.attach(limitSwPin);
  debouncer.interval(90);

  digitalWrite(enablePin, HIGH); // Turns the motor on
}

void loop() {
  // ***** READ ENCODER *****
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition); // DEBUG - Disable eventually
  }
  // **************************

 // **** Limit Switch ****
   debouncer.update();
   if ( debouncer.fell() ) { // if limit switch is pushed (it will go to LOW, because pullup)
    myEnc.write(0); // writes 0 to the encoder location
    Serial.println("Limit Switch Activated"); // DEBUG
  }


  // **** Serial handling  ****
  if (stringComplete) {
    Serial.println(inputString);

    // command to manual move: moveTo(X,Y,Z)
    // X = Speed (0-255), Y direction (1or2), Z location )
    // moveTo(120,1,3000)

    if (inputString.startsWith("moveTo")) { // if the string begins with moveTo - Case Sensitive!
      int commaPosition1 = inputString.indexOf(','); //gets the location of the first comma
      String cmdSpeedSt = inputString.substring(7, commaPosition1); // cuts the string between the 7th char and the first comma
      int cmdSpeed = cmdSpeedSt.toInt(); // casts the SPEED it into an int
      Serial.print("cmdSpeed: ");
      Serial.println(cmdSpeed);

      String cmdDirectionSt = inputString.substring(commaPosition1 + 1, commaPosition1 + 2); // gets the direction
      int cmdDirection = cmdDirectionSt.toInt(); // casts the DIRECTION into an int
      Serial.print("cmdDirection: ");
      Serial.println(cmdDirection);

      //String cmdRest = inputString.substring(commaPosition1 + 2);
      //      Serial.print("cmdRest: "); // DEBUG
      //      Serial.println(cmdRest); // DEBUG

      int commaPosition2 = inputString.lastIndexOf(','); // gets the location of the last comma
      int parePosition = inputString.indexOf(')'); //gets the location of the closing parentheses
      //      Serial.print("parePosition: "); Serial.println(parePosition); // DEBUG

      String cmdPositionSt = inputString.substring(commaPosition2 + 1, parePosition); // cuts the string between the last comma to the parentheses
      cmdPosition = cmdPositionSt.toInt(); //casts the TARGET POSITION into a global variable (long actually, which will accept an int)
      Serial.print("cmdPosition: ");
      Serial.println(cmdPosition);

      // engage the motor via the moveTo Function
      moveTo(cmdSpeed, cmdDirection, cmdPosition);

    }

    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  // stop the motor
  if (((newPosition + 40 >= cmdPosition) && (newPosition - 40 <= cmdPosition )) && (motionDone == 0)) {
    Serial.println("Done Moving");
    analogWrite(motorSpeedPin, 0);
    motionDone = 1;
  }
}

void move(int speed, int direction) { // General Move with Direction (no target location). Speed needs to be 0-255, Direction 1 or 2

  motionDone = 0;

  if (direction == 1) { // clockwise
    digitalWrite(CWPin, HIGH);
    digitalWrite(CCWPin, LOW);
  } else if (direction == 2) { // counter-clockwise
    digitalWrite(CWPin, LOW);
    digitalWrite(CCWPin, HIGH);
  } else {
    digitalWrite(CWPin, LOW);
    digitalWrite(CCWPin, LOW);
    Serial.println("Bad Direction, use 1 for CW and 2 for CCW");
    return;
  }

  analogWrite(motorSpeedPin, speed);
  Serial.println("Moving with speed " + speed);
}

void moveTo(int speed, int direction, long goToPosition) { // Move to a specific location ******

  // print the command
  Serial.print("Command Received: Speed: ");
  Serial.print(speed);
  Serial.print("\t Direction: ");
  Serial.print(direction);
  Serial.print("\t goToPosition: ");
  Serial.println(goToPosition);

  motionDone = 0; // set the flag for movement (not done is zero means it's moving)

  if (direction == 1) { // clockwise
    digitalWrite(CWPin, HIGH);
    digitalWrite(CCWPin, LOW);
  } else if (direction == 2) { // counter-clockwise
    digitalWrite(CWPin, LOW);
    digitalWrite(CCWPin, HIGH);
  } else {
    digitalWrite(CWPin, LOW);
    digitalWrite(CCWPin, LOW);
    Serial.println("Bad Direction, use 1 for CW and 2 for CCW");
    return;
  }

  analogWrite(motorSpeedPin, speed); //move the motor

}

// **** Serial Functions ****
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}