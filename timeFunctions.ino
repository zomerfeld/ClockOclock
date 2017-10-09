
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
    cmdPosition = newPosition + distanceMinute;
    moveTo(255,1,cmdPosition);
    Serial.println("moving a minute");
    lastMinute = 0.5;
  }
  if (nowMinute > lastMinute) {
    cmdPosition = newPosition + distanceMinute;
    moveTo(255,1,cmdPosition);
    Serial.println("moving a minute");
    lastMinute = nowMinute;
    rtc.now();
  }
}

void fiveSecMove () {
    cmdPosition = newPosition + distance5Second;
    moveTo(255,1,cmdPosition);
    Serial.println("moving 5 sec");
    rtc.now();
  }
  

