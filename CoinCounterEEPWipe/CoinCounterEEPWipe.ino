#include <EEPROM.h>

int last = 0;
int five = 0;
int ten = 0;
int twenty = 0;
int fifty = 0;
int one = 0;
int two = 0;

void setup() {
  // put your setup code here, to run once:
        Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
Serial.println("starting");   
}

void loop() {
  // put your main code here, to run repeatedly: 
  
      Serial.println("Initializing Coin Counts.......");
    five = (EEPROM.read(1) * 256 + (EEPROM.read(2)));
    Serial.println(five);  
    ten = (EEPROM.read(3) * 256 + (EEPROM.read(4)));
    Serial.println(ten);
    twenty = (EEPROM.read(5) * 256 + (EEPROM.read(6)));
    Serial.println(twenty);
    fifty = (EEPROM.read(7) * 256 + (EEPROM.read(8)));
    Serial.println(fifty);
    one = (EEPROM.read(9) * 256 + (EEPROM.read(10)));
    Serial.println(one);
    two = (EEPROM.read(11) * 256 + (EEPROM.read(12)));
    Serial.println(two);
    Serial.println("Coin Totals Updated From EEPROM");   
    
    delay(1);
          EEPROM.write (1, 0);
           delay(1000);
           Serial.println(".");
          EEPROM.write (2, 0); 
           delay(1000);
          Serial.println("..");
          EEPROM.write (3, 0);
           delay(1000);
          Serial.println("...");
          EEPROM.write (4, 0);
           delay(1000);
          Serial.println("....");
          EEPROM.write (5, 0);
           delay(1000);
          Serial.println(".....");
          EEPROM.write (6, 0);
           delay(1000);
          Serial.println("......");
          EEPROM.write (7, 0);
           delay(1000);
          Serial.println(".......");
          EEPROM.write (8, 0);
           delay(1000);
          Serial.println("........");
          EEPROM.write (9, 0);
           delay(1000);
          Serial.println(".........");
          EEPROM.write (10, 0);
           delay(1000);
          Serial.println("..........");
          EEPROM.write (11, 0);
           delay(1000);
          Serial.println("...........");
          EEPROM.write (12, 0);
          delay(1000);
          Serial.println("............*");
          Serial.println("EEPROM Values reset to 0 for Coin Counter 5000");
          
              Serial.println("Initializing Coin Counts.......");
    five = (EEPROM.read(1) * 256 + (EEPROM.read(2)));
    Serial.println(five);  
    ten = (EEPROM.read(3) * 256 + (EEPROM.read(4)));
    Serial.println(ten);
    twenty = (EEPROM.read(5) * 256 + (EEPROM.read(6)));
    Serial.println(twenty);
    fifty = (EEPROM.read(7) * 256 + (EEPROM.read(8)));
    Serial.println(fifty);
    one = (EEPROM.read(9) * 256 + (EEPROM.read(10)));
    Serial.println(one);
    two = (EEPROM.read(11) * 256 + (EEPROM.read(12)));
    Serial.println(two);
    Serial.println("Coin Totals Updated From EEPROM");   
          
          delay(500000000);
}
