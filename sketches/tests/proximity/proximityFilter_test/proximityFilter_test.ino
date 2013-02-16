#define SENSOR_PIN A0

const double q = 50; //process variance
const double r = 50; //measure variance
double x; // estimated state
double p; //error variance
double k; //Kalman gain

void setup() {
  Serial.begin(9600);
  x = analogRead(SENSOR_PIN);
  p = 0;
  Serial.println("[Sensor, Filter]: ");
}

void loop() {
  int z = analogRead(SENSOR_PIN);
  p = p + q;
  k = p/(p+r);
  x = x + k*(z-x); //filtering
  p = (1-k)*p;
  Serial.print(z); //measured
  Serial.print(",");
  Serial.println(x); //filtered
  delay(50);
}
