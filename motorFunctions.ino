void pwmOut(int out) {
  if (out > 0) {                         // if REV > encoderValue motor move in forward direction.
    analogWrite(motorSpeedPin, out);         // Enabling motor enable pin to reach the desire angle
    forward();                           // calling motor to move forward
    //Serial.print("Moving FWD: ");
    //Serial.println(out);
    checkMax(); //stops the motor from moving past the max point. 
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
}

void checkMax() { //stops the motor if it's trying to go over the max position 
  if (encoderValue + 40 >= maxPosition) {
    stopMotor();
    motionDone = 1;
    Serial.println("Reached Max");
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

  Serial.println("FINDING EDGES");
  delay(3000);
  while ((analogRead(limitSwPin) <= magnetHigh) && (analogRead(limitSwPin) >= magnetLow)) { // numbers might need adjusting based on analog reads of hall sensor
    Serial.println("Running");
    pwmOut(-150);
    Serial.print("Sensor: ");
    Serial.println(analogRead(limitSwPin));
    newPosition = encoderValue;
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      Serial.print ("encoder position: "); // DEBUG - Disable eventually
      Serial.println(newPosition); // DEBUG - Disable eventually
    }
  }

  stopMotor();

  encoderValue = 0; // writes 0 to the encoder location
  Serial.println("Limit Switch Activated - MIN"); // DEBUG
  Serial.print("New Limit: "); // DEBUG
  Serial.println(encoderValue);

  pwmOut(100); // move FWD a bit
  delay(2000); //


  while ((analogRead(limitSwPin) <= magnetHigh) && (analogRead(limitSwPin) >= magnetLow)) { // numbers might need adjusting based on analog reads of hall sensor
    pwmOut(150); // move FWD
    newPosition = encoderValue;
    if (newPosition != oldPosition) {
      oldPosition = newPosition;
      Serial.print ("encoder position: "); // DEBUG - Disable eventually
      Serial.println(newPosition); // DEBUG - Disable eventually
    }
  }

  maxPosition = newPosition;
  stopMotor();
  Serial.print("MAX Position: ");
  Serial.println(maxPosition); // DEBUG - Disable eventually
  PPR = maxPosition;
  delay(3000);
  Serial.println("Moving to 0 point");
  setpoint = 0;

}
