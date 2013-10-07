#include <EEPROM.h>
#include <U8glib.h>

//DHT22 Temp and Humidity
#include "DHT.h"
#define DHTPIN 6     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// gLCD
U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);
// SPI Com Nano 3.0: SCK = en = 13, MOSI = rw = 11, CS = di = 10 Note these differ to which Arduino you are using.

int incomingByte = 0;	// for incoming serial data
float total = 0; // $ value of coins
int coinstotal = 0; //total coins
int last = 0; //value of last coin in cents
float fiftyWorth = 0;
int five = 0;
int ten = 0;
int twenty = 0;
int fifty = 0;
int one = 0; //$1
int two = 0; //$2

int mode = 0;
//new add buttons and functions
//hold button time for function
int allow;
//constants
const int buttonPin1 = 2;
const int buttonPin2 = 3;
const int buttonPin3 = 4;
const int ledPin = 13;

//Variables
int ledState = HIGH;
int buttonState1 = LOW;
int buttonState2 = LOW;
int buttonState3;
int lastButtonState1 = LOW;
int lastButtonState2 = LOW;
int lastButtonState3 = LOW;
int buttonReading1 = LOW;
int buttonReading2 = LOW;
int manualFiftyToAdd = 0;

long lastDebounceTime1 = 0;
long lastDebounceTime2 = 0;
long debounceDelay = 3000; //hold button time for function

int eepreads; //eeprom reads
int eepwrites; //eeprom writes

int fivegraph = 0;
int tengraph = 0;
int twentygraph = 0;
int fiftygraph = 0;
int onegraph = 0;
int twograph = 0;
int x;
int text;
String c;
char tchar[10];
String combo;
void startup(void) {
  int x; //width of string in font pixels
  int p; // pixel to centre on
  p = 64;
  u8g.setFont(u8g_font_courR10);
  //Serial.print(x); //debug
  u8g.drawFrame(0,0,128,64); //border around screen
  u8g.setFontPosCenter();
  x = u8g.getStrWidth("Coin Box 1000");
  u8g.drawStr(centreText(64,x), 10, "Coin Box 1000");
  x = u8g.getStrWidth("Please");
  u8g.drawStr(centreText(p = 64,x), 45, "Please");
  x = u8g.getStrWidth("Insert Coin");
  u8g.drawStr(centreText(64,x), 56, "Insert Coin");
}
  
void draw(void) {
  String dollar = "$";
  // graphic commands to redraw the complete screen
  u8g.setFont(u8g_font_lucasfont_alternate);
  u8g.drawFrame(0,0,128,64); //border around screen
  u8g.drawBox(2,(61-fivegraph),19,(fivegraph)); // graph draws * 6
  u8g.drawBox(23,(61-tengraph),19,(tengraph));
  u8g.drawBox(44,(61-twentygraph),19,(twentygraph));
  u8g.drawBox(65,(61-fiftygraph),19,(fiftygraph));
  u8g.drawBox(86,(61-onegraph),19,(onegraph));
  u8g.drawBox(107,(61-twograph),19,(twograph));
  
  float valt = total/100;
  String d = "$";
  char buffer [10];  
  String tem = dtostrf(valt, 4, 2, buffer);
  char carray[tem.length() + 1]; //determine size of the array
  tem.toCharArray(carray, sizeof(carray)); //put readStringinto an array
  Serial.print("float to string ");Serial.println(carray);
 String combo = d + carray;
  // String comb = dollar();  //function experiments that dont work :(   
  Serial.print("combine strings ");Serial.println(combo);
  char carrayy[combo.length() + 1]; //determine size of the array
  combo.toCharArray(carrayy, sizeof(carrayy)); //put readStringinto an array
  Serial.print("combo string to char ");Serial.println(carrayy);
  u8g.drawStr(centreText(64,x = u8g.getStrWidth(carrayy)),9,carrayy);
  
  u8g.setFontPosCenter();
  u8g.setColorIndex(0);
  itoa (five, tchar, 10);
  u8g.drawStr(centreText(12,x = u8g.getStrWidth(tchar)),58,tchar);
  itoa (ten, tchar, 10);
  u8g.drawStr(centreText(12+21,x = u8g.getStrWidth(tchar)),58,tchar);
  itoa (twenty, tchar, 10);
  u8g.drawStr(centreText(12+42,x = u8g.getStrWidth(tchar)),58,tchar);
  itoa (fifty, tchar, 10);
  u8g.drawStr(centreText(12+63,x = u8g.getStrWidth(tchar)),58,tchar);
  itoa (one, tchar, 10);
  u8g.drawStr(centreText(12+84,x = u8g.getStrWidth(tchar)),58,tchar);
  itoa (two, tchar, 10);
  u8g.drawStr(centreText(12+106,x = u8g.getStrWidth(tchar)),58,tchar);
  u8g.setColorIndex(1);
}
////////////////////////////////////////////////////////////////////
 void dollar(float val) {
  float valt = val/100;
  String d = "$";
  char buffer [10];   
  String tem = dtostrf(valt, 5, 2, buffer);
  char carray[tem.length() + 1]; //determine size of the array
  tem.toCharArray(carray, sizeof(carray)); //put readStringinto an array
  Serial.print("float to string ");Serial.println(carray);
  String combo = d + carray;     
  Serial.print("combine strings ");Serial.println(combo);
  char carrayy[combo.length() + 1]; //determine size of the array
  combo.toCharArray(carrayy, sizeof(carrayy)); //put readStringinto an array
  Serial.print("combo string to char ");Serial.println(carrayy);
  combo = combo;
//return combo;
}
////////////////////////////////////////////////////////////////////
void fiftydraw(void){
  u8g.setFont(u8g_font_lucasfont_alternate);
  u8g.drawFrame(0,0,128,64); //border around screen
  u8g.drawStr(centreText(64,x = u8g.getStrWidth("Manually Add Fifties")),10,"Manually Add Fifties");
  //u8g.setPrintPos(3,20);
  //u8g.print("How many Fifty cent coins"); 
 // u8g.drawStr(centreText(64,x = u8g.getStrWidth("Manually Add Fifties")),10,"Manually Add Fifties");
 // u8g.setPrintPos(3,40);
 // u8g.print(" would you like to add?");
 
  u8g.setPrintPos(40,55);
  u8g.print(manualFiftyToAdd);   
 
  itoa (fiftyWorth, tchar, 10);
  u8g.drawStr(centreText(60,x = u8g.getStrWidth(tchar)),55,tchar);
}

void fiftycent(void) {
  Serial.println("Fifty cent mode!");
  u8g.firstPage(); //update display
  do {
  fiftydraw();
  } while( u8g.nextPage() );
  delay(100);
  mode = 1;
  while(mode = 1) {
      delay(100);
      buttonState2 = digitalRead(buttonPin2);
      buttonState1 = digitalRead(buttonPin1);  
      //Serial.println(buttonState1); Serial.println(buttonState2);
    if((buttonState2 == HIGH) && (buttonState1 == LOW)) {
      manualFiftyToAdd += 1;
      fiftyWorth = (manualFiftyToAdd*50)/100;
      char buffer[10];
      String tem = dtostrf(fiftyWorth, 5, 2, buffer);
      u8g.firstPage(); //update display
      do {
      fiftydraw();
      } while( u8g.nextPage() );
      Serial.println("Fifty Draw Screen Complete"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)  
      delay(100);
      Serial.println(manualFiftyToAdd);
      Serial.println(fiftyWorth);
      }
        else if((buttonState1 == HIGH) && (buttonState2 == LOW)) {
        fifty += manualFiftyToAdd;
        Serial.print("$");Serial.print(fiftyWorth);Serial.println(" added to MoneyBox");
        manualFiftyToAdd = 0;
        Serial.println("50 Cent Amount reset to 0");
        u8g.firstPage(); //update display
        do {
        fiftydraw();
        } while( u8g.nextPage() );
        Serial.println("Fifty Draw 2 Screen Complete"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)  
        delay(100);
        }
          else if((buttonState1 == HIGH) && (buttonState2 == HIGH)) {
          mode = 0;
          u8g.firstPage(); //update display
          do {
          startup();
          } while( u8g.nextPage() );
          Serial.println("Draw Start Screen Complete"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)   
          Serial.println("Mode 0");
          return;
          }
    }
}

boolean debounce(int pin) {
  boolean state;
  boolean previousState;
  
  previousState = digitalRead(pin);
  for(int counter=0; counter < debounceDelay; counter++) {
    delay(1);
    state = digitalRead(pin);
    if(state != previousState) {
      counter = 0;
      previousState = state;
    }
  }
  return state;
}
//////////////////////////////
int centreText(int pixel, int text) { //pixel = pixel to centre on. text equals width to centre
//int width; 
int ycoord;
//width = text;
//Serial.println(width);
ycoord = pixel - (text/2);
//Serial.print("Y CoOrd for centre text: ");Serial.println(ycoord); //debug
return ycoord;
}
//////////////////////////////

void setup() {
    Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
    dht.begin();
    mode = 0; //reset mode 0 after reset
    pinMode(buttonPin1, INPUT);
    pinMode(buttonPin2, INPUT);
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
    Serial.print("Total 5 Cent Coins = ");Serial.println(five);
    
      if (EEPROM.read(3) * 256 + (EEPROM.read(4)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
        ten = 0;
        eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
        }
        else{
          ten = (EEPROM.read(3) * 256 + (EEPROM.read(4)));
           eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
      }  
    Serial.print("Total 10 Cent Coins = ");Serial.println(ten);
    
        if (EEPROM.read(3) * 256 + (EEPROM.read(4)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
          twenty = 0;
          eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
          }
        else{
          twenty = (EEPROM.read(3) * 256 + (EEPROM.read(4)));
          eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
      }  
    Serial.print("Total 20 Cent Coins = ");Serial.println(twenty);
    
        if (EEPROM.read(7) * 256 + (EEPROM.read(8)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
        fifty = 0;
        eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
        }
        else{
          fifty = (EEPROM.read(7) * 256 + (EEPROM.read(8)));
           eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
      }  
    Serial.print("Total 50 Cent Coins = ");Serial.println(fifty);
  
        if (EEPROM.read(9) * 256 + (EEPROM.read(10)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
        one = 0;
        eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
        }
        else{
          one = (EEPROM.read(9) * 256 + (EEPROM.read(10)));
           eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
      }  
    Serial.print("Total 1 Dollar Coins = ");Serial.println(one);
  
        if (EEPROM.read(11) * 256 + (EEPROM.read(12)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
        two = 0;
        eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
        }
        else{
          two = (EEPROM.read(11) * 256 + (EEPROM.read(12)));
           eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
      }  
    Serial.print("Total 2 Dollar Coins = ");Serial.println(two);
    Serial.println("Coin Totals Updated From EEPROM");    
    Serial.println("Serial port ready....");
  
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
  startup();
  } while( u8g.nextPage() );
  Serial.println("Draw Start Screen Complete"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)    
}


void loop() {

if (Serial.available() > 0) { // send data only when you receive data:
  count();
  }
    else if (debounce(buttonPin1)) {
    fiftycent();
     }
  }


void count(void) {
        incomingByte = Serial.read(); // read the incoming byte:
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
  fiftygraph = map(fifty,0,coinstotal,1,62);
  onegraph = map(one,0,coinstotal,1,62);
  twograph = map(two,0,coinstotal,1,62);
    
  u8g.firstPage(); //update display
  do {
  draw();
  } while( u8g.nextPage() );
  Serial.println("draw"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)
}

void Temp(void) {
    float t = dht.readTemperature();
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");  
}

void Humidity(void) {
    float h = dht.readHumidity();
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %");
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

  



