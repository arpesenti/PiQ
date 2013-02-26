#define SENSOR_PIN A0

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
}

void loop() {
  int d = analogRead(A1);
  Serial.println(d);
  delay(200);
}
