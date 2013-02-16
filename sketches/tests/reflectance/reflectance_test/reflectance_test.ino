//  Lower output voltage is an indication of greater reflection

#define SENSOR_PIN A0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int r = analogRead(SENSOR_PIN);
  Serial.println(r);
  delay(100);
}