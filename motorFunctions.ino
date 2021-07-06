void pwmOut(int out) {
  if (out > 0) {                         // if REV > encoderValue motor move in forward direction.
    analogWrite(motorSpeedPin, out);         // Enabling motor enable pin to reach the desire angle
    forward();                           // calling motor to move forward
    //Serial.print("Moving FWD: ");
    //Serial.println(out);
    //    checkMax(); //stops the motor from moving past the max point.
  }
  else {
    analogWrite(motorSpeedPin, abs(out));          // if REV < encoderValue motor move in forward direction.
    //    Serial.print("Moving BACK: ");
    //    Serial.println(out);
    reverse();                            // calling motor to move reverse

  }
}


void forward () {
  digitalWrite(CWPin, LOW);
  digitalWrite(CCWPin, HIGH);
  //  Serial.println("forward()");
}

void reverse () {
  digitalWrite(CWPin, HIGH);
  digitalWrite(CCWPin, LOW);
}

void finish () {
  digitalWrite(CWPin, LOW);
  digitalWrite(CCWPin, LOW);
}

void stopMotor() {
  //Serial.println("Done Moving");
  analogWrite(motorSpeedPin, 0);
  motionDone = 1;
  //  Serial.println("**STOPPED**");

}

void checkMax() { //stops the motor if it's trying to go over the max position
  if ((encoderValue + 40 >= maxPosition) & (newPosition >= 60))  {
    Serial.println("Reached Max");
    stopMotor();
    motionDone = 1;
    setpoint = 0;

  }
}

void motorSpeed(int reqSpeed) {
  reqSpeed = abs(reqSpeed);
  myPID.SetOutputLimits(-reqSpeed, reqSpeed);
}

void findEdges () {
  //This function finds the edges for the motor movements - Max and Min
  // uses a limit switch (or a hall magnet switch)
  // it will go all the way back, mark the zero point and then all the way forward (find the max point)
  // then it goes to a specific angle between (see setpoint below)
  // this can be triggered manually by putting "reset" into the serial command.

  Serial.println("FINDING EDGES");
  Alarm.delay(3000);
  while ((analogRead(limitSwPin) <= magnetHigh) && (analogRead(limitSwPin) >= magnetLow)) { // numbers might need adjusting based on analog reads of hall sensor

    Serial.println("Running");
    pwmOut(-150);
    Alarm.delay(0);
    Serial.print("Sensor: ");
    Serial.println(analogRead(limitSwPin));
    newPosition = encoderValue;
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      //      Serial.print ("encoder position: "); // DEBUG - Disable eventually
      //      Serial.println(newPosition); // DEBUG - Disable eventually
    }
  }

  Serial.println("*** FOUND MIN EDGE ***"); // DEBUG - Disable eventually
  stopMotor();
  encoderValue = 250; // writes 0 to the encoder location
  Serial.println("Limit Switch Activated - MIN"); // DEBUG
  Serial.print("New Limit: "); // DEBUG
  Serial.println(encoderValue);

  pwmOut(100); // move FWD a bit
  Alarm.delay(2000); //

  while ((analogRead(limitSwPin) <= magnetHigh) && (analogRead(limitSwPin) >= magnetLow)) { // move only when not on limits. Does this even work??? NZ
    // numbers might need adjusting based on analog reads of hall sensor
    pwmOut(150); // move FWD
    Alarm.delay(0);
    Serial.println("FINDING MAX POINT"); // DEBUG - Disable eventually
    newPosition = encoderValue;
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      Serial.print ("encoder position: "); // DEBUG - Disable eventually
      Serial.println(newPosition); // DEBUG - Disable eventually
      Serial.print("FE: LIMIT - HALL SENSOR READING: ");
      Serial.println(analogRead(limitSwPin));

    }
  }

  maxPosition = newPosition + 50;
  Serial.println("*** FOUND MAX EDGE ***"); // DEBUG - Disable eventually
  stopMotor();
  Serial.print("MAX Position: ");
  Serial.println(maxPosition); // DEBUG - Disable eventually
  PPR = maxPosition;
  Alarm.delay(3000); // wait 3 seconds
  Serial.println("Moving to minAngle deg");
  REV = map (10, minAngle, maxAngle, 0, PPR); // replace first number with destination you want to go after mapping.
  setpoint = REV; //Destination in revolutions - PID will work to achive this value consider as SET value  Serial.println("Moved to Min");
  return;

}
