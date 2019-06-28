# ClockOclock
Rachel's Clock Project


## USAGE

### BOOTUP SEQUENCE
* When the radio boots up (or when serial opens) it goes through a calibration sequence:
** It goes back to try to find the minimum point (it uses magnets and a hall sensor as a limit switch)
** Then it goes forward to find the max point
** Then it goes to a predetermined location (currently 90°)
* It then maps the minimum point to 10 and the max point to 170 and you can now request it to go to specific angles based on that mapping. 


### MANUAL CONTROL
The board has two physical buttons that will move the motors back and forward. In addition to that, you can also move the dial with the 
* You can also control it via a serial connection, baud rate 250000
#### Commands
* *<any number>* - after the bootup sequence, this number will work as an angle for the front dial. 
** Do not put any number lower than 10 or higher than 170 or the motor will try to take it there by force. 
* *reset* - runs the calibration sequence again and finds the endges 
* *f* - moves the motor forward a bit
* *b* - moves the motor backwards a bit




## FAQ / TROUBLESHOOTING


## THINGS THAT NEED REWORK
* Mapping isn't accurate (10-170 might need to be adjusted) 
* The board doesn't tell the difference between max and min limit switches currently
* The values for the hall sensor limit switch might need adjusting when you move the radio around to more or less magnetically noise environments. 
