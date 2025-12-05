int PIN_BUZZER = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_BUZZER, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  tone(PIN_BUZZER, 880);
  digitalWrite(PIN_BUZZER, LOW);
  delay(100);
  noTone(PIN_BUZZER);
  tone(PIN_BUZZER, 550);
  delay(1000);

}
