
// **** Time Functions ****

void showTime() { // show current time
  digitalClockDisplay();

  //also show some debug notes.
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
  REV = map (0, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void moveSec() { // *** NOT READY YET
  Serial.println("****** Second Moving ********");
  destAngle = destAngle+secondAngle; //adds the X angle that equal second to the angle. 
  REV = map(destAngle, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV; //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move90fm () {
  Serial.println("****** IT'S TIME ********");
  incrementalToggle = false;  // Disable incremental move
  motorSpeed(150); // Set Speed 
  destAngle = 37; // Destination Angle
  REV = map (destAngle, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV; //Destination in revolutions - PID will work to achive this value consider as SET value

  Serial.println("****** time to move by second ********"); //now its time to move  by x degrees per second to till the next timer
  secondSpeed = 95; // Set Speed for incremental
  secondAngle = ((74-25)/360); // Calculate new value for second angle
  Serial.print("Tiny move Angle: ");
  Serial.println(secondAngle);
  incrementalToggle = false; // NOT READY YET - Change to TRUE to move the incremental change - not ready yet! 
}

void move100fm () {
  Serial.println("****** IT'S TIME ********");
  REV = map (115, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move550am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (12, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move600am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (30, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}


void move700am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (56, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}

void move800am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (73, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}


void move900am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (92, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}


void move1000am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (104, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
  setpoint = REV;                    //Destination in revolutions - PID will work to achive this value consider as SET value
}


void move1100am () {
  Serial.println("****** IT'S TIME ********");
  REV = map (117, minAngle, maxAngle, minPosition, PPR); // mapping degree into pulse
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

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
