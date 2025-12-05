# Workout-Rep-Counter
EMG-driven rep counter using Arduino and LCD interface. It calibrates baseline muscle signals, detects contraction peaks, decrements reps in real time, and transitions to a completion state when the set ends.

# Project Set Up
## Installation/Hardware Specifications
### Software Installation:
* Install the [`Arduinio IDE`](https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE)
* Install the LiquidCrystal library. You can do this through the Library Manager page in the Arduino IDE

### Hardware Specifications 
* Arduinio UNO R3
* LCD 1602 Module
* Potentiometer
* EMG Muscle Sensor
* EMG Cord (Usually comes with EMG Sensor)
* 3 disposable surface electrodes
* 2 9V batteries
* Active Buzzer
* 4 buttons

## Hardware Connections
### Connecting LCD to Arduino
Make the following pin connections, from LCD to Adrduino (LCD -> Arduino):
* VSS -> GND
* VDD -> 5V
* RS -> digital pin 8
* RW -> GND
* E -> digital pin 9
* D4 -> digital pin 10
* D5 -> digital pin 11
* D6 -> digital pin 12
* D7 -> digital pin 13
* A -> 5V (There is a 220 ohm resistor in series in this connection)
* K -> GND 
### LCD/Potentiometer Connections
You would also have to make these connections:
* V0 (pin from LCD) -> signal (potentiometer)
* VCC (potentiometer) -> 5V (Arduino)
* GND (potentiometer) -> GND (Arduino)
### Button Connections 
Make the following pin connections, from the 4 buttons to Adrduino (Buttons -> Arduino):
* Button 1 -> digital pin 7
* Button 2 -> digital pin 6
* Button 3 -> digital pin 5
* Button 4 -> digital pin 4
* All buttons should also connect to GND
 


