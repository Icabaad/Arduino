
#include <EEPROM.h>
  int a = 1000;
  int low = 0;
  int high = 0;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
}

void loop() {
  // put your main code here, to run repeatedly: 
  

 // Serial.println(EEPROM.read(lowByte(1)));
  //a += 1;
  //Serial.println(lowByte(a));
  //Serial.println(highByte(a));
  //EEPROM.write(1, a);
  //Serial.println(EEPROM.read(lowByte(1)));
  //delay(1000);
  
 // Serial.print("Old value");
  //Serial.println(a);
  //a += 1;
  //int high = highByte(a);
  //int low = lowByte(a);
  
  
  //int word = high * 256 + low;
  //Serial.print("New reformed word ");
  //Serial.println(a);
  
 /*reset coin counter
          EEPROM.write (1, 0);
          EEPROM.write (2, 0); 
          EEPROM.write (3, 0);
          EEPROM.write (4, 0);
          EEPROM.write (5, 0);
          EEPROM.write (6, 0);
          EEPROM.write (7, 0);
          EEPROM.write (8, 0);
          EEPROM.write (9, 0);
          EEPROM.write (10, 0);
          EEPROM.write (11, 0);
          EEPROM.write (12, 0);
          
          Serial.println("EEPROM Values reset to 0 for Coin Counter 5000");
}
/*



