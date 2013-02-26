//  Lower output voltage is an indication of greater reflection

#define SENSOR_PIN A0

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
}

void loop() {
  int r = analogRead(SENSOR_PIN);
  Serial.println(r);
  delay(2000);
  digitalWrite(LOW);
  delay(2000);
  //digitalWrite(5, HIGH);
  //delay(1000);
  //digitalWrite(5, LOW);
  //delay(1000);
  delay(100);
}
