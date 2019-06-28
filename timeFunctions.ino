
// **** Time Functions ****

void showTime() {
  digitalClockDisplay();

  //debug - prints the hall sensor read on limit switch
  Serial.print("hall sensor read = ");
  Serial.println(analogRead(limitSwPin));

  // debug - print an enc read
  Serial.print ("encoder position: "); // DEBUG - Disable eventually
  Serial.println(newPosition); // DEBUG - Disable eventually

//  debug - print an MaxPosition
  Serial.print ("max position: "); // DEBUG - Disable eventually
  Serial.println(maxPosition-50); // DEBUG - Disable eventually

}

// ************ TIMED MOVEMENTS *****************

void moveRest () {
  Serial.println("****** IT'S TIME ********");
  REV = map (minAngle, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void moveSec() { // *** NOT READY YET
  Serial.println("****** Second Moving ********");
  destAngle = destAngle+secondAngle;
  REV = map(destAngle, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV; //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move90fm () {
  Serial.println("****** IT'S TIME ********");
  incrementalToggle = false;  // Disable incremental move
  motorSpeed(150); // Set Speed 
  destAngle = 25; // Destination Angle
  REV = map (destAngle, minAngle, maxAngle, 0, PPR); // mapping degree into pulse
  setpoint = REV; //Destination in revolutions - PID will work to achive this value consider as SET value

  Serial.println("****** time to move by second ********");
  secondSpeed = 95; // Set Speed for incremental
  secondAngle = ((74-25)/360); // Calculate degrees -- rewrite this 
  Serial.print("Tiny move Angle: ");
  Serial.println(secondAngle);
  incrementalToggle = false // NOT READY YET - Change to TRUE to move the incremental change - not ready yet! 
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
