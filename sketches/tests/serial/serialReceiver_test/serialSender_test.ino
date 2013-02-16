void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        pinMode(13, OUTPUT);  
}

void loop() {
  Serial.println("Hello");
  delay(500);
}
