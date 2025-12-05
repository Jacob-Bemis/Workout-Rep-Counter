enum deviceState{IDLE, CALIBRATION_WEIGHT, CALIBRATION_REPS, MONITORING, REP_COUNT, BUZZER, SET_FINISHED, COUNTDOWN_START};

deviceState currentState = IDLE;

const int START_BUTTON_PIN = 7;
const int STOP_BUTTON_PIN = 4;
const int INCRE_BUTTON_PIN = 6;
const int DECR_BUTTON_PIN = 5;

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



void setup() {
  // put your setup code here, to run once:  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(INCRE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DECR_BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  lcd.begin(16, 2);

}

void loop() {
  // put your main code here, to run repeatedly:
  switch (currentState){
    Serial.println(currentState);
   

  case IDLE:
    LCD_Display_IDLE();
    reps = 1;

    for (auto &b : btns) {
      if (checkButtonPressed(b)) {
        
        if (b.pin == START_BUTTON_PIN) {
          lcd.clear();
          currentState = CALIBRATION_WEIGHT;
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
            currentState = COUNTDOWN_START ;
        }else if(b.pin == STOP_BUTTON_PIN){
            lcd.clear();
            currentState = CALIBRATION_WEIGHT;
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

}
