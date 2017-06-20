
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

  if (now.second() >= lastSecond+5) {
    long destPosition = newPosition + 300;
    moveTo(255, 1, destPosition);
    lastSecond = now.second();
    Serial.println(now.second(), DEC);
    Serial.println("moving a second");
  }

}
