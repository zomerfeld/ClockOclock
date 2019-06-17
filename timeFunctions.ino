
// **** Time Functions ****

void showTime() {
  digitalClockDisplay();

  //debug - prints the hall sensor read on limit switch
  Serial.print("hall sensor read = ");
  Serial.println(analogRead(limitSwPin));

  // debug - print an enc read
  Serial.print ("encoder position: "); // DEBUG - Disable eventually
  Serial.println(newPosition); // DEBUG - Disable eventually

//  // debug - print an MaxPosition
//  Serial.print ("max position: "); // DEBUG - Disable eventually
//  Serial.println(maxPosition); // DEBUG - Disable eventually


  //
  //  if (now.second() >= lastSecond+5) { //this condition doesn't work
  //    long destPosition = newPosition + 300;
  //    moveTo(255, 1, destPosition);
  //    lastSecond = now.second();
  //    Serial.println(now.second(), DEC);
  //    Serial.println("moving a second");
  //  }

}

// ************ TIMED MOVEMENTS *****************

void moveRest () {
  Serial.println("****** IT'S TIME ********");
  REV = map (minAngle, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move90fm () {
  Serial.println("****** IT'S TIME ********");
  REV = map (92, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move100fm () {
  Serial.println("****** IT'S TIME ********");
  REV = map (102, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move550am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (12, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move600am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (20, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}


void move700am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (48, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move800am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (65, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}


void move900am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (81, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}


void move1000am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (100, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}


void move1100am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (110, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}




// **********************************************

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
