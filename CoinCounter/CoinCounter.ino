#include <EEPROM.h>


int incomingByte = 0;	// for incoming serial data
float total = 0;
int last = 0;

int five = 0;
int ten = 0;
int twenty = 0;
int fifty = 0;
int one = 0;
int two = 0;
void setup() {
	Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
        Serial.println("test");
}

void loop() {

	// send data only when you receive data:
    if (Serial.available() > 0) 
        {
        // read the incoming byte:
	incomingByte = Serial.read();
	// say what you got:
	Serial.print("I received: ");
	Serial.println(incomingByte, DEC);
        last = (incomingByte);
          if(last == 5)
          {
          (five) += 1;
          Serial.print("total 5 cent= ");
          Serial.println(five);
          //EEPROM.write (1, highByte(five));
          // EEPROM.write (2, lowByte(five));
          }
          else if(last == 10)
          {
          ten += 1;
          Serial.print("total 10 cent= ");
          Serial.println(ten);
       //   EEPROM.write (3, highByte(ten));
       //  EEPROM.write (4, lowByte(ten));
          }
          else if(last == 20)
          {
          twenty += 1;
          Serial.print("total 20 cent= ");
          Serial.println(twenty);
          EEPROM.write (5, highByte(twenty));
          EEPROM.write (6, lowByte(twenty));          
          }
          else if(last == 50)
          {
          fifty += 1;
          Serial.print("total 50 cent= ");
          Serial.println(fifty);
          EEPROM.write (7, highByte(fifty));
          EEPROM.write (8, lowByte(fifty));
          }
          else if(last == 100)
          {
          one += 1;
          Serial.print("Total 1 Dollar Coins= ");
          Serial.println(one*100);
          EEPROM.write (9, highByte(one));
          EEPROM.write (10, lowByte(one));
          }
          else if(last == 2)
          {
          two += 1;
          Serial.print("Total 2 Dollar Coins= ");
          Serial.println(two*100);
          EEPROM.write (11, highByte(two));
          EEPROM.write (12, lowByte(two));
          }
          else
          {
          Serial.print("Unrecognized!");
          }
          
    Serial.print("last coin = ");
    Serial.println(last);
    total = (five * 5) + (ten * 10) + (twenty * 20) + (fifty * 50) + (one * 100) + (two * 200);  
    //total += (last);
    Serial.print("$$$= $");
    Serial.println(total/100);
    Serial.print("Total Coins= ");
    Serial.println(five + ten + twenty + fifty + one + two);
    }
}
