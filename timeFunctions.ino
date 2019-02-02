
// **** Time Functions ****

void showTime() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Serial.print(" UNIX TIME = ");
  Serial.print(now.unixtime());
  Serial.println();

  nowHour = now.hour();
  nowMinute = now.minute();
  nowSecond = now.second();

  //debug - prints the hall sensor read on limit switch
  Serial.print("hall sensor read = ");
  Serial.println(analogRead(limitSwPin));

  // debug - print an enc read
  Serial.print ("encoder position: "); // DEBUG - Disable eventually
  Serial.println(newPosition); // DEBUG - Disable eventually

  // debug - print an MaxPosition
  Serial.print ("max position: "); // DEBUG - Disable eventually
  Serial.println(maxPosition); // DEBUG - Disable eventually


  //
  //  if (now.second() >= lastSecond+5) { //this condition doesn't work
  //    long destPosition = newPosition + 300;
  //    moveTo(255, 1, destPosition);
  //    lastSecond = now.second();
  //    Serial.println(now.second(), DEC);
  //    Serial.println("moving a second");
  //  }

}

void minuteMove () {
  //  Serial.print("minute: ");
  //  Serial.println (nowMinute, DEC);
  if (nowMinute == 0) {
    int pushMove = map (distanceMinute, 0, angle, 0, PPR); // mapping degree into pulse
    setpoint = encoderValue + pushMove;
    Serial.println("moving a minute");
    lastMinute = 0.5;
  }
  if (nowMinute > lastMinute) {
    setpoint = encoderValue + distanceMinute;
    Serial.println("moving a minute");
    lastMinute = nowMinute;
    rtc.now();
  }
}

void fiveSecMove () {
  //    if (motionDone == 1) {
  int pushMove = map (distance5Second, 0, angle, 0, PPR); // mapping degree into pulse
  
  setpoint = encoderValue + pushMove;
  Serial.print("Moving 5 seconds: ");
  Serial.print(distance5Second); 
  Serial.print(" deg, ");
  Serial.print(pushMove);
  Serial.println(" revolutions");
  //    } else {
  //      Serial.println("Already in Motion");
  //    }
  rtc.now();
}
