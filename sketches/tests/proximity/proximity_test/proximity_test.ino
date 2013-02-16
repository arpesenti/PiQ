#define SENSOR_PIN A0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int d = analogRead(SENSOR_PIN);
  Serial.println(d);
  delay(200);
}
