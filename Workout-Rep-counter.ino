#include <LiquidCrystal.h>
struct Btn;

enum deviceState{IDLE, CALIBRATION_WEIGHT, CALIBRATION_REPS, MONITORING, REP_COUNT, BUZZER, SET_FINISHED, COUNTDOWN_START};

deviceState currentState = IDLE;

const int START_BUTTON_PIN = 7;
const int STOP_BUTTON_PIN = 4;
const int INCRE_BUTTON_PIN = 6;
const int DECR_BUTTON_PIN = 5;
const int BUZZER_PIN = 3;

int emg_Pin = A3;
int emg_Value =0;
int raw;
int emg;
static bool buzzerEntered = false;
static unsigned long riseStart = 0;

bool repInit = false;
int nuRep;
bool lock = false;
static bool above;

unsigned long lastRepTime = 0;                    // timestamp of last counted rep
const unsigned long REP_MIN_INTERVAL = 700;     // minimum ms between reps (tune 300-800)
const float FALL_FACTOR = 0.25;                   // reset level multiplier (hysteresis)

const int DATA_SIZE = 6;
int dataBuffer[DATA_SIZE];
int dataIndex = 0;
long dataSum = 0;
bool dataInitialized = false;
static int baseline =0;
int risingThreshold;
int fallingThreshold;


int getSmoothedEMG(int raw) {
  if (!dataInitialized) {
    // initialize buffer with first sample
    for (int i = 0; i < DATA_SIZE; i++){
      dataBuffer[i] = raw;
    } 
    dataSum = (long)raw * DATA_SIZE;
    dataInitialized = true;
    dataIndex = 0;
    return raw;
  }

  dataSum -= dataBuffer[dataIndex];
  dataBuffer[dataIndex] = raw;
  dataSum += dataBuffer[dataIndex];
  dataIndex = (dataIndex + 1) % DATA_SIZE;
  return (int)(dataSum / DATA_SIZE);
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
{STOP_BUTTON_PIN, HIGH, HIGH, 0, 0}, 
{INCRE_BUTTON_PIN, HIGH, HIGH, 0, 0}, 
{DECR_BUTTON_PIN, HIGH, HIGH, 0, 0}, };

bool checkButtonPressed(Btn &b) {
  now = millis();
  bool currentRead = digitalRead(b.pin);

  // Update debounce
  if (currentRead != b.lastRead) {
    b.lastChange = now;
    b.lastRead = currentRead;
  }

  // If stable long enough, update stable state
  if ((now - b.lastChange) > DEBOUNCE_DELAY) {
    if (currentRead != b.lastStable) {
      b.lastStable = currentRead;

      // Detect falling edge (HIGH → LOW)
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

int weightLevel = 0; //weightLevel == 0 --> light weight
                      // weightLevel == 1 --> heavy weight
int reps = 1;
int threshold;
int heavyWeight = 110; //threshold for heavy weight will be determined
int lightWeight = 10; //threshold for light weight will be determined

void LCD_Display_IDLE(){
  lcd.setCursor(0, 0);
  lcd.print(" Start Workout ");
}

void LCD_Display_CALIBRATION_WEIGHT(){
  if (weightLevel == 0){
    lcd.setCursor(0, 0);
    lcd.print("Heavy Weight");
    lcd.setCursor(0, 1);
    lcd.print("Light Weight");
    lcd.setCursor(13, 1);
    lcd.print("<--");
  }else{
    lcd.setCursor(0, 0);
    lcd.print("Heavy Weight");
    lcd.setCursor(0, 1);
    lcd.print("Light Weight");
    lcd.setCursor(13, 0);
    lcd.print("<--");
  }
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
    baseline = getSmoothedEMG(analogRead(emg_Pin));
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
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(INCRE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DECR_BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(BUZZER_PIN, OUTPUT);
  //lcd.print(" Start Workout ");
}




void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println(currentState);
  static deviceState lastState;
  if (lastState != currentState){
    Serial.print("STATE = ");
    Serial.println(currentState);
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


  case CALIBRATION_WEIGHT:
    LCD_Display_CALIBRATION_WEIGHT();
    for (auto &b : btns) {
      if (checkButtonPressed(b)) {
        if (b.pin == START_BUTTON_PIN) {
          threshold = (weightLevel == 0) ? lightWeight : heavyWeight;
          lcd.clear();
          currentState = CALIBRATION_REPS;
        } else if (b.pin == STOP_BUTTON_PIN) {
          lcd.clear();
          currentState = IDLE;
        } else if (b.pin == INCRE_BUTTON_PIN) {
          lcd.clear();
          weightLevel = 1;
        } else if (b.pin == DECR_BUTTON_PIN) {
          lcd.clear();
          weightLevel = 0;
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
        }else if(b.pin == STOP_BUTTON_PIN){
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
    LCD_Display_COUNTDOWN_START();
    lcd.clear();
    repInit = false;
    currentState = MONITORING;
    break;

    case MONITORING:
    LCD_Display_MONITORING();
    raw = analogRead(emg_Pin);
    emg = getSmoothedEMG(raw);
    

    if (!above) {
      baseline = (baseline * 0.995) + (emg * 0.005);  // smooth, slow adaptation
    }
    //Serial.print("baseline = "); Serial.println(baseline);
   // Serial.print("emg = "); Serial.println(emg);
    risingThreshold = baseline   *1.18;   //must rise above 18%
    fallingThreshold = baseline  * 0.80;     // must fall below 80% baseline

    Serial.print("EMG=");   Serial.print(emg);
    Serial.print(" | baseline="); Serial.print(baseline);
    Serial.print(" | rise="); Serial.print(baseline * 1.15);
    Serial.print(" | fall="); Serial.print(baseline * 0.80);
    Serial.print(" | above="); Serial.println(above);

    // --- Rising phase must be sustained ---
    

    if (!above && emg >= risingThreshold) {
      if (riseStart == 0) riseStart = millis();
      if (millis() - riseStart > 120) {      // hold above threshold 120ms
          above = true;
          Serial.println("VALID CONTRACTION START");
      }
  } else {
        riseStart = 0;
    }

    if (!above && emg >= risingThreshold) {
      Serial.println("Above risingThreshold");
      Serial.print("emg = "); Serial.println(emg);
      above = true;
    }

    if (above && emg < fallingThreshold) {           
      if (millis() - lastRepTime > REP_MIN_INTERVAL) {
          reps--;                                 // <-- now reps WILL decrease
          lastRepTime = millis();
          repInit = true;

          tone(BUZZER_PIN, 880, 120);
          Serial.println("*** REP COUNTED ***");
          Serial.print("reps = "); Serial.println(reps);
        }
      above = false;
    }

    if (repInit){
      if (millis() - lastRepTime > 3000) {
        lcd.clear();
        lock = false;
        repInit = false;
        lastRepTime = millis();
        above = false;
        currentState = BUZZER;
        break;
      }
    }

    if (reps <= 0) {
        lcd.clear();
        Serial.println(">>> REPS HIT ZERO - GOING TO SET_FINISHED <<<");
        currentState = SET_FINISHED;
      
      }
    break;

    case SET_FINISHED:
    Serial.println(">>> INSIDE SET_FINISHED <<<");
    lcd.clear();
    LCD_SET_FINISHED();
    delay(4000);
    lcd.clear();
    currentState = IDLE;   // <-- STOP EVERYTHING so LCD CAN'T BE OVERWRITTEN
    break;

    case BUZZER:
    if (!buzzerEntered) {
        buzzerEntered = true;
        above = false;
        repInit = false;
        lastRepTime = millis();
        Serial.println("=== BUZZER STATE RESET ===");
    }
    tone(BUZZER_PIN, 440);
    LCD_BUZZER();
    raw = analogRead(emg_Pin);
    emg = getSmoothedEMG(raw);
   
    Serial.print("baseline = "); Serial.println(baseline);

    if (!above) {
      baseline = (baseline * 0.995) + (emg * 0.005);  // smooth, slow adaptation
    }
    risingThreshold = baseline * 1.18; // must rise above 18%
    fallingThreshold = baseline * 0.80; // must fall below 80% baseline

    if (!above && emg >= risingThreshold) {
      Serial.println("Above risingThreshold");
      Serial.print("emg = "); Serial.println(emg);
      above = true;
    }

    if (above && emg < fallingThreshold) {           
      if (millis() - lastRepTime > REP_MIN_INTERVAL) {
          reps--;                                 // <-- now reps WILL decrease
          lastRepTime = millis();
           buzzerEntered = false;

          noTone(BUZZER_PIN);
          Serial.println("*** REP COUNTED ***");
          Serial.print("reps = "); Serial.println(reps);
          lcd.clear();
          repInit = true;
          currentState = MONITORING;
          lock = true;
        }
      above = false;
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
        Serial.println(">>> REPS HIT ZERO - GOING TO SET_FINISHED <<<");
        currentState = SET_FINISHED;
      
      }
    break;


  }


}
