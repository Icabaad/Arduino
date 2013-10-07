

int incomingByte = 0;	// for incoming serial data

void setup() {
	Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
        Serial1.begin(9600);
 
}

void loop() {
     
  Serial1.write(20);
  delay(1000);
     Serial1.write(5);
            Serial.println(5);
     delay(1000);
     Serial1.write(2);
            Serial.println(2);
     delay(1000);
     Serial1.write(50);
            Serial.println(50);
     delay(1000);
     Serial1.write(20);
            Serial.println(20);
               Serial1.write(50);
            Serial.println(50);
     Serial1.write(20);
            Serial.println(20);	
      
     
}
