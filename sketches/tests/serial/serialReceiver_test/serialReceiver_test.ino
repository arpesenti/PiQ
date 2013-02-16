int incomingByte = 0;   // for incoming serial data

void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        pinMode(13, OUTPUT);  
}

void loop() {
        digitalWrite(13, LOW);
        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();
                digitalWrite(13, HIGH);
                delay(100);
                digitalWrite(13, LOW);
                // say what you got:
                Serial.print("I received: ");
                Serial.println(incomingByte, DEC);
        }
}
