#include <LiquidCrystal.h>
struct Btn;

enum deviceState{IDLE, CALIBRATION_REPS, MONITORING, BUZZER, SET_FINISHED, COUNTDOWN_START};

deviceState currentState = IDLE;

const int START_BUTTON_PIN = 7;
const int BACK_BUTTON_PIN = 4;
const int INCRE_BUTTON_PIN = 6;
const int DECR_BUTTON_PIN = 5;
const int BUZZER_PIN = 3;

int emg_Pin = A3;
int emgReading;
int emgAvg;
static bool buzzerEntered = false;
static unsigned long riseStart = 0;

bool repInit = false;
int nuRep;
bool lock = false;
static bool contraction;

unsigned long lastRepTime = 0;                    // timestamp of last counted rep
const unsigned long REP_MIN_INTERVAL = 700;     // minimum time between reps 

const int DATA_SIZE = 6;
int dataBuffer[DATA_SIZE];
int dataIndex = 0;
long dataSum = 0;
bool dataInitialized = false;
static int baseline =0;
int risingThreshold;
int fallingThreshold;


int movingAverageEMG(int emgReading) {
  if (!dataInitialized) {
    for (int i = 0; i < DATA_SIZE; i++){
      dataBuffer[i] = emgReading; // initialize buffer with first sample data
    } 
    dataSum = (long)emgReading * DATA_SIZE;
    dataInitialized = true;
    dataIndex = 0;
  } else{
    dataSum -= dataBuffer[dataIndex]; // Remove oldest EMG reading from dataSum
    dataBuffer[dataIndex] = emgReading; // Replace the oldest EMG reading with the new
    dataSum += dataBuffer[dataIndex]; // Add this value to dataSum
    dataIndex = (dataIndex + 1) % DATA_SIZE; // Change the index of the buffer
  }
  return (int)(dataSum / DATA_SIZE); // calculate the average EMG reading 
}



const unsigned long DEBOUNCE_DELAY = 20;
const uint32_t FIRE_DELAY = 150;
uint32_t now;

struct Btn { 
    int pin; 
    bool lastStable, lastRead; 
    uint32_t lastChange, lastFire; 
};

Btn btns[] = {{START_BUTTON_PIN, HIGH, HIGH, 0, 0}, 
{BACK_BUTTON_PIN, HIGH, HIGH, 0, 0}, 
{INCRE_BUTTON_PIN, HIGH, HIGH, 0, 0}, 
{DECR_BUTTON_PIN, HIGH, HIGH, 0, 0}, };

bool checkButtonPressed(Btn &b) {
  now = millis();
  bool currentRead = digitalRead(b.pin);

  if (currentRead != b.lastRead) {
    b.lastChange = now;
    b.lastRead = currentRead;
  }

  if ((now - b.lastChange) > DEBOUNCE_DELAY) {
    if (currentRead != b.lastStable) {
      b.lastStable = currentRead;

     
      if (b.lastStable == LOW && (now - b.lastFire) > FIRE_DELAY) {
        b.lastFire = now;
        return true; // pressed event
      }
    }
  }

  return false;
}


const int LCD_rs = 8, LCD_en = 9, LCD_d4 = 10, LCD_d5 = 11, LCD_d6 = 12, LCD_d7 = 13;
LiquidCrystal lcd(LCD_rs, LCD_en, LCD_d4, LCD_d5, LCD_d6, LCD_d7); 

int reps = 1;



void LCD_Display_IDLE(){
  lcd.setCursor(0, 0);
  lcd.print(" Start Workout ");
}


void LCD_Display_CALIBRATION_REPS(){
    lcd.setCursor(0, 0);
    lcd.print(" Number of ");
    lcd.setCursor(0, 1);
    lcd.print(" reps: ");
    lcd.setCursor(6, 1);
    lcd.print(reps);
}

void LCD_Display_COUNTDOWN_START(){
  for (int x = 3; x > 0; x--){
    lcd.setCursor(0,0);
    lcd.print("Workout will");
    lcd.setCursor(0,1);
    lcd.print("start in: ");
    lcd.setCursor(10,1);
    lcd.print(x);
    baseline = movingAverageEMG(analogRead(emg_Pin));
    delay(1500);
  }
}

void LCD_Display_MONITORING(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Reps left in ");
  lcd.setCursor(0,1);
  lcd.print("workout: ");
  lcd.setCursor(9,1);
  lcd.print(reps);
  
}

void LCD_SET_FINISHED(){
  if (reps <= 0){
    lcd.setCursor(0,0);
    lcd.print("Congrats!");
    lcd.setCursor(0,1);
    lcd.print("Set Complete");
  }else{
    lcd.setCursor(0,0);
    lcd.print("Set Failed");
    lcd.setCursor(0,1);
    lcd.print(" :( ");
  }
}

int num = 0;
void LCD_BUZZER(){
  lcd.setCursor(num,0);
  lcd.print("PUSH!!!!!");
  num++;
  if (num > 5){
    num--;
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BACK_BUTTON_PIN, INPUT_PULLUP);
  pinMode(INCRE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DECR_BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(BUZZER_PIN, OUTPUT);
}




void loop() {
  static deviceState lastState;
  if (lastState != currentState){
    lastState = currentState;
  }
  switch (currentState){
   

  case IDLE:
    LCD_Display_IDLE();
    reps = 1;

    for (auto &b : btns) {
      if (checkButtonPressed(b)) {
        
        if (b.pin == START_BUTTON_PIN) {
          lcd.clear();
          currentState = CALIBRATION_REPS;
        }
      }
    }
    break;


    case CALIBRATION_REPS:
    LCD_Display_CALIBRATION_REPS();
  //  delay(100);
    for (auto &b : btns) {
    if (checkButtonPressed(b)) {
        if (b.pin == START_BUTTON_PIN){
            lcd.clear();
            nuRep = reps;
            currentState = COUNTDOWN_START ;
        }else if(b.pin == BACK_BUTTON_PIN){
            lcd.clear();
            currentState = IDLE;
        }else if (b.pin == INCRE_BUTTON_PIN){
            lcd.clear();
            reps++;
        }else if (b.pin == DECR_BUTTON_PIN){
            if (reps != 1){
                lcd.clear();
                reps--;
            }
          }
        }
    }
    break;

    case COUNTDOWN_START:
    for (auto &b : btns) {
      if (checkButtonPressed(b)) {
        
        if (b.pin == BACK_BUTTON_PIN) {
          lcd.clear();
          currentState = CALIBRATION_REPS;
        }
      }
    }
    LCD_Display_COUNTDOWN_START();
    lcd.clear();
    repInit = false;
    currentState = MONITORING;
    break;

    case MONITORING:
    LCD_Display_MONITORING();
    emgReading = analogRead(emg_Pin);
    emgAvg = movingAverageEMG(emgReading);
    Serial.println(emgAvg);
    

    if (!contraction) {
      baseline = (baseline * 0.995) + (emgAvg * 0.005);  // baseline adapts
    }

    risingThreshold = baseline   * 1.18;   //must rise above 18%
    fallingThreshold = baseline  * 0.90;     // must fall below 90% baseline
    

    if (!contraction && emgAvg >= risingThreshold) {
      if (riseStart == 0) {
        riseStart = millis();
      }

      if (millis() - riseStart > 120) { 
        contraction = true;
      }
    } else if (emgAvg < risingThreshold) {
        riseStart = 0;
      }
   
    if (contraction && emgAvg < fallingThreshold) {           
      if (millis() - lastRepTime > REP_MIN_INTERVAL) {
          reps--;                                 // <-- now reps WILL decrease
          lastRepTime = millis();
          repInit = true;

          tone(BUZZER_PIN, 880, 120);
        }
      contraction = false;
    }

    if (repInit){
      if (millis() - lastRepTime > 3000) {
        lcd.clear();
        lock = false;
        repInit = false;
        lastRepTime = millis();
        contraction = false;
        currentState = BUZZER;
        break;
      }
    }

    if (reps <= 0) {
        lcd.clear();
        currentState = SET_FINISHED;
      
      }
    break;

    case SET_FINISHED:
    lcd.clear();
    LCD_SET_FINISHED();
    delay(4000);
    lcd.clear();
    currentState = IDLE;  
    break;

    case BUZZER:
    if (!buzzerEntered) {
        buzzerEntered = true;
        contraction = false;
        repInit = false;
        lastRepTime = millis();
    }
    tone(BUZZER_PIN, 440);
    LCD_BUZZER();
    emgReading = analogRead(emg_Pin);
    emgAvg = movingAverageEMG(emgReading);
    Serial.println(emgAvg);
   

    if (!contraction) {
      baseline = (baseline * 0.995) + (emgAvg * 0.005);  // smooth, slow adaptation
    }
    risingThreshold = baseline * 1.15; // must rise above 18%
    fallingThreshold = baseline * 0.90; // must fall below 80% baseline

    if (!contraction && emgAvg >= risingThreshold) {
      contraction = true;
    }

    if (contraction && emgAvg < fallingThreshold) {           
      if (millis() - lastRepTime > REP_MIN_INTERVAL) {
          reps--;                                 // <-- now reps WILL decrease
          lastRepTime = millis();
           buzzerEntered = false;

          noTone(BUZZER_PIN);
          lcd.clear();
          repInit = true;
          currentState = MONITORING;
          lock = true;
        }
      contraction = false;
    }
    if (!lock){
      if (millis() - lastRepTime > 5000) {
        noTone(BUZZER_PIN);
        lcd.clear();
        currentState = SET_FINISHED;
        break;
      }
    }

    if (reps <= 0) {
        lcd.clear();
        currentState = SET_FINISHED;
      }
    break;


  }


}
