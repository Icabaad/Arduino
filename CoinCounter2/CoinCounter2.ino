#include <EEPROM.h>
#include <U8glib.h>

U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);
// SPI Com Nano 3.0: SCK = en = 13, MOSI = rw = 11, CS = di = 10

int incomingByte = 0;	// for incoming serial data
float total = 0;
int last = 0;
int five = 0;
int ten = 0;
int twenty = 0;
int fifty = 0;
int one = 0;
int two = 0;
int eepromwrites = 0; //for later code after adding a reset switch

int fivegraph;
int tengraph;
int twentygraph;
int fiftygraph;
int onegraph;
int twograph;

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here 
  //u8g.setFont(u8g_font_courB12);
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  //u8g.drawStr( 2, 2, agraph);
  u8g.drawFrame(0,0,128,64);
  u8g.drawBox(2,(62-fivegraph),19,(fivegraph));
  u8g.drawBox(22,(62-tengraph),19,(tengraph));
  u8g.drawBox(41,(62-twentygraph),19,(twentygraph));
  u8g.drawBox(60,(62-fiftygraph),19,(fiftygraph));
  u8g.drawBox(79,(62-onegraph),19,(onegraph));
  u8g.drawBox(98,(62-twograph),19,(twograph));
  
  u8g.setPrintPos(3,15);
  u8g.print(fivegraph); 
  u8g.setPrintPos(22,15);
  u8g.print(tengraph); 
}

void setup() {
    Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
    u8g.setRot180();      // flip screen, if required
    u8g.setColorIndex(1); // pixel on
    Serial.println("Initializing Coin Counts.......");
   // five = (EEPROM.read(1) * 256 + (EEPROM.read(2)));
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
    Serial.println("Serial port ready....");
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
          EEPROM.write (3, highByte(ten));
          EEPROM.write (4, lowByte(ten));
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
    
//update display. Edit top function    
  u8g.firstPage(); 
  do {
  draw();
  } while( u8g.nextPage() );
// rebuild the picture after some delay

  fivegraph = map(five,0,100,0,50);
  tengraph = map(ten,0,100,0,50);
  twentygraph = map(twenty,0,100,0,50);
  fiftygraph = map(fifty,0,100,0,50);
  onegraph = map(one,0,100,0,50);
  twograph = map(two,0,100,0,50);

delay(500);
 
} 
 /*reset coin counter
 
 //remember to take note of total coins to update total eeprom writes and update/write code for it :)
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
*/

  



