#include <EEPROM.h>
#include <U8glib.h>

U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);
// SPI Com Nano 3.0: SCK = en = 13, MOSI = rw = 11, CS = di = 10 Note these differ to which Arduino you are using.

int incomingByte = 0;	// for incoming serial data
float total = 0; // $ value of coins
int coinstotal = 0; //total coins
int last = 0; //value of last coin in cents

int five = 0;
int ten = 0;
int twenty = 0;
int fifty = 0;
int one = 0; //$1
int two = 0; //$2

int eepreads; //eeprom reads
int eepwrites; //eeprom writes

int fivegraph = 0;
int tengraph = 0;
int twentygraph = 0;
int fiftygraph = 0;
int onegraph = 0;
int twograph = 0;

void draw(void) {
  // graphic commands to redraw the complete screen
  //u8g.setFont(u8g_font_courB12);
  //u8g.setFont(u8g_font_unifont);***
  u8g.setFont(u8g_font_courR08);
  //u8g.drawStr( 2, 2, agraph);
  u8g.drawFrame(0,0,128,64); //border around screen
  u8g.drawBox(2,(62-fivegraph),19,(fivegraph)); // graph draws * 6
  u8g.drawBox(23,(62-tengraph),19,(tengraph));
  u8g.drawBox(44,(62-twentygraph),19,(twentygraph));
  u8g.drawBox(65,(62-fiftygraph),19,(fiftygraph));
  u8g.drawBox(86,(62-onegraph),19,(onegraph));
  u8g.drawBox(107,(62-twograph),19,(twograph));
  
  
  u8g.setPrintPos(3,10);
  u8g.print("MoneyBox V.6"); 
  u8g.setPrintPos(83,10);
  u8g.print("$");("%"); 
  u8g.setPrintPos(88,10);
  u8g.print(total/100); 
  u8g.setColorIndex(0);
  u8g.setPrintPos(3,61);
  u8g.print((five/coinstotal)*100);  
  u8g.setPrintPos(25,61);
  u8g.print((ten/coinstotal)*100);
  u8g.setPrintPos(47,61);
  u8g.print(twenty/coinstotal*100);
  u8g.setPrintPos(69,61);
  u8g.print(fifty/coinstotal*100); 
  u8g.setPrintPos(91,61);
  u8g.print(one/coinstotal*100); 
  u8g.setPrintPos(114,61);
  u8g.print(two/coinstotal*100);   
  u8g.setColorIndex(1);
}

void setup() {
    Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
    
    u8g.setRot180();      // flip screen, if required
    u8g.setColorIndex(1); // pixel on
    Serial.print("Coin Check = ");
    Serial.println(coinstotal); //debug
    Serial.println("Initializing Coin Counts From Memory.......");
    //read previous values from EEPROM after restart
    if (EEPROM.read(1) * 256 + (EEPROM.read(2)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
      five = 0;
      eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
      }
       else{
       five = (EEPROM.read(1) * 256 + (EEPROM.read(2)));
       eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
       }  
    Serial.print("Total 5 Cent Coins = ");   
    Serial.println(five);
  /*
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
    */
  coinstotal = (five + ten + twenty + fifty + one + two);  //Total Coin counts. Auto scales graph height. 
    if (coinstotal <= 0) {  //0 value on first start defies laws of Mathmatics. Screws graphs.
      coinstotal = 1;
       }
      else{      
      coinstotal = (five + ten + twenty + fifty + one + two);
      Serial.print("Coin Count: ");
      Serial.println(coinstotal); //debug
    }
    
  u8g.firstPage(); //update display
  do {
  draw();
  } while( u8g.nextPage() );
  Serial.println("draw"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)    
   
}
void loop() {


    // send data only when you receive data:
    if (Serial.available() > 0) 
        {
        // read the incoming byte:
	incomingByte = Serial.read();
        last = (incomingByte);
	// say what you got:
	Serial.print("I received a ");
	Serial.print(incomingByte, DEC);
        if (last < 3){
          Serial.println(" Dollar Coin!");
          }
          else{
            Serial.println(" Cent Coin!");
          }

          if(last == 5){
          five += 1;
          Serial.print("Total 5c Coins= ");
          Serial.println(five);
          //EEPROM.write (1, highByte(five));
          // EEPROM.write (2, lowByte(five));
          }
          else if(last == 10){
          ten += 1;
          Serial.print("Total 10c Coins= ");
          Serial.println(ten);
      //    EEPROM.write (3, highByte(ten));
      //    EEPROM.write (4, lowByte(ten));
          }
          else if(last == 20)
          {
          twenty += 1;
          Serial.print("Total 20c Coins= ");
          Serial.println(twenty);
     //     EEPROM.write (5, highByte(twenty));
     //     EEPROM.write (6, lowByte(twenty));          
          }
          else if(last == 50)
          {
          fifty += 1;
          Serial.print("Total 50c Coins= ");
          Serial.println(fifty);
       //   EEPROM.write (7, highByte(fifty));
      //    EEPROM.write (8, lowByte(fifty));
          }
          else if(last == 1)
          {
          one += 1;
          Serial.print("Total 1 Dollar Coins= ");
          Serial.println(one);
      //    EEPROM.write (9, highByte(one));
      //    EEPROM.write (10, lowByte(one));
          }
          else if(last == 2)
          {
          two += 1;
          Serial.print("Total 2 Dollar Coins= ");
          Serial.println(two);
        //  EEPROM.write (11, highByte(two));
        //  EEPROM.write (12, lowByte(two));
          }
          else
          {
          Serial.print("Unrecognized!");
          }
          
    total = (five * 5) + (ten * 10) + (twenty * 20) + (fifty * 50) + (one * 100) + (two * 200);
    Serial.print("$$$= $");
    Serial.println(total/100);
    coinstotal = (five + ten + twenty + fifty + one + two);
    Serial.print("Total Coins= ");
    Serial.println(coinstotal);
    Serial.print("Last Coin Entered = ");
    Serial.println(last);
    
  //map total coin values to range 1-62 so graph doesnt overflow Change 62 to max height you want
  fivegraph = map(five,0,coinstotal,1,62);
  tengraph = map(ten,0,coinstotal,1,62);
  twentygraph = map(twenty,0,coinstotal,1,62);
  fiftygraph = map((fifty),0,coinstotal,1,62);
  onegraph = map(one,0,coinstotal,1,62);
  twograph = map(two,0,coinstotal,1,62);
    
  u8g.firstPage(); //update display
  do {
  draw();
  } while( u8g.nextPage() );
  Serial.println("draw"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)
  
}
 /*   
 //update display. Edit top function    
  u8g.firstPage(); 
  do {
  draw();
  } while( u8g.nextPage() );
  Serial.println("draw");
// rebuild the picture after some delay
// delay(500);
*/
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

  



