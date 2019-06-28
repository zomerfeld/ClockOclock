# ClockOclock
Rachel's Clock Project


## USAGE

### BOOTUP SEQUENCE
* When the radio boots up (or when serial opens) it goes through a calibration sequence:
  * It goes back to try to find the minimum point (it uses magnets and a hall sensor as a limit switch)
  * Then it goes forward to find the max point
  * Then it goes to a predetermined location (currently 90°)
* The board will then map the minimum point to 10 and the max point to 170 (temp values). You can now request it to go to specific angles based on that mapping. 


### MANUAL CONTROL
The board has two physical buttons that will move the motors back and forward. In addition to that, you can also move the dial with the 
* You can also control it via a serial connection, baud rate 250000
#### Commands
* **_\<any number\>_** - after the bootup sequence, this number will work as an angle for the front dial. 
  * Do not put any number lower than 10 or higher than 170 or the motor will try to take it there by force. 
* **_reset_** - runs the calibration sequence again and finds the endges 
* **_f_** - moves the motor forward a bit
* **_b_** - moves the motor backwards a bit

### Alarms
The board is currently set to run two functions on a schedule:
* the function _showTime_ every 5 seconds 
* the function _move90fm_ on a specific time

You can add timers or move the scheduled times under the section "Test Alarms" in the Setup function:
```
  Alarm.alarmRepeat(15, 52, 0, move90fm);
  Alarm.timerRepeat(5, showTime); // show time every 5 seconds

  moveSecAl = Alarm.timerRepeat(6, moveSec); // move every 6 seconds -- disabled currently 
```


## FAQ / TROUBLESHOOTING
#### Wanted ot change the max and min Values?
Change these:
```
int minAngle = 10; // Maximum degree of motion.
int maxAngle = 170; // Maximum degree of motion.
```

## THINGS THAT NEED REWORK
* Mapping isn't accurate (10-170 might need to be adjusted) 
* The board doesn't tell the difference between max and min limit switches currently
* The values for the hall sensor limit switch might need adjusting when you move the radio around to more or less magnetically noise environments. 
