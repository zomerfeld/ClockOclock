
// **** Serial Functions ****

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    readString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {

      stringComplete = true;


    }
  }
}

void clrSrString() {
    // clear the string:
  readString = ""; // Cleaning User input, ready for new Input
  stringComplete = false;
}
