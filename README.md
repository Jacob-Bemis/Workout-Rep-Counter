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
### Buzzer Connections
Make the following pin connections, from the active buzzer to Adrduino (Buzzer -> Arduino):
* VCC -> digital pin 3
* GND -> GND
### Battery Connections 
Make these connections with the 2 9V batteries
* Connect the negative terminal of one battery to the positive terminal of the other battery with a snap on battery connector. This creates a common ground
* Connect the positive terminal of a bettery to the +V pin of the EMG sensor
* Connect the negative terminal of a battery to the -V pin of the EMG sensor
### EMG Sensor Connections to Arduino
Make the following pin connections, from the EMG sensor to Adrduino (EMG -> Arduino):
* SIG -> A3
* GND -> GND

# Using the Project
Here is step-by-step how to use the Rep counter
1. Connect Arduino to computer and upload the script named "Workout-Rep-Counter.ino" 
2. Place the three electrodes on your bicep
  * Place one electrode on the upper part of the muscle belly of your  bicep
  * Place another electrode roughly 2 cm below the previouse one
  * Place the final electrode on your elbow or any other boney part of your arm
3. Connect the EMG cord to the sensor and connect the cord to the electrodes based on the color:
  * Red -> upper part of the muscle belly
  * Green -> 2 cm below Red
  * Yellow -> Boney part
4. To start a workout press Button 1
  * This will bring you to the next stage where you will be configuring the number of reps
5. Pressing Button 2 will increase the number of reps, and pressing Button 3 will decrease the number of reps, caping it at a 1 rep minumum
  * The number of reps will appear on the LCD display
  * If you would like to go back to the previouse stage, press Button 4
6. Press Button 1 to continue to the next stage which is the countdown
  * As soon as you press Button 1, stand up straight and hold a weight with your arm in a resting position (your arm is straight down)
7. After the countdown is finished, you may begin your workout:
  * The first curl you do is the initial curl, which starts the workout
  * Continue doing curls until the the number of reps you set before reaches 0. After this point the workout is over and will transition back to the beginning 
  * After the first initial rep, if you don't do a rep within 3 seconds between each rep, the device will transition into a different state. In this state you have 5 seconds to do a rep, if you fail to do a rep the workout is over and a message will display saying that you failed the workout. If you manage to get another rep in, it will transition back to the previous state and the workout will continue, or if this is your last rep, the workout will be over
    
 
    


