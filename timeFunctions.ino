
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

  int t_min = now.minute();
//  Serial.println(t_min);

    // *** IF TIME ***
    // This is where you put the timed commands. 
    // For example the part below is moving the thing a bit
  if (t_min % 3 == 0 && motionDone == 1) {
    cmdPosition = cmdPosition+90;
    moveTo(100,1,cmdPosition);
  }


}
