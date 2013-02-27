#define SENSOR_PIN A0

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
}

void loop() {
  Serial.println(analogRead(A10));
  Serial.println(analogRead(A12));
  Serial.println(analogRead(A14));
  Serial.println("---------------");
  delay(500);
}
