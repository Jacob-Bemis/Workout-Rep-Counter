int emg_Pin = A2;
int emg_Value =0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  emg_Value = analogRead(emg_Pin);
  Serial.println(emg_Value);
  delay(200);

}
