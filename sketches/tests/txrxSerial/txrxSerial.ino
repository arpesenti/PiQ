


// Setup runs at startup and is used configure pins and init system variables
void setup()
{
  Serial.begin(9600);  

 
}


void loop()
{
   Serial.write(45); // send a byte with the value 45

   int bytesSent = Serial.write("hello");
   
   delay(2000);
}



