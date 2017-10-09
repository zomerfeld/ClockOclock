void move(int speed, int direction) { // General Move with Direction (no target location). Speed needs to be 0-255, Direction 1 or 2

  if (motorDisabled == 1) {
    Serial.println("Motor disabled, can't move");
    return;
  }
  
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

  if (motorDisabled == 1) {
    Serial.println("Motor disabled, can't move");
    return;
  }

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

void stopMotor() {
    Serial.println("Done Moving");
    analogWrite(motorSpeedPin, 0);
    motionDone = 1;
}


void findEdges () { //This function finds the edges for the motor movements - Max and Min
  // uses a limit switch (or a hall magnet switch)
  
}

