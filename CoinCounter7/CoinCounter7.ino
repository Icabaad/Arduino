// DangerCode V1.0 19/05/2013
// This code is free to use where you like but please credit and reference the website where appropriate. 
//http://www.dangertech.org

//Libraries
#include <EEPROM.h>
#include <U8glib.h>

//DHT22 Temp and Humidity
#include "DHT.h"
#define DHTPIN 6     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// Graphics LCD
U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);
// SPI Com Nano 3.0: SCK = en = 13, MOSI = rw = 11, CS = di = 10 Note these differ to which Arduino you are using.

int incomingByte = 0;	// for incoming serial data
int coinstotal = 0; //total coins
int last = 0; //value of last coin in cents
int five = 0; //5c
int ten = 0; //10c
int twenty = 0;
int fifty = 0;
int one = 0; //$1
int two = 0; //$2
float total = 0; // $ value of coins

//fifty cent manual add ints
float fiftyWorth = 0;
int mode = 0;
int manualFiftyToAdd = 0;

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
long lastDebounceTime1 = 0;
long lastDebounceTime2 = 0;
long debounceDelay = 3000; //hold button time for function

//memory read/writes
int eepreads; //eeprom reads
int eepwrites; //eeprom writes


int timer; //graph screen timeout to stat screen

//graph ints
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

////////////////////////////////////////////////////
void startup(void) {
  int x; //width of string in font pixels
  int p; // pixel to centre on
  p = 64; //set pixel to centre on middle of screen
  u8g.setFont(u8g_font_courR10); //font
  u8g.drawFrame(0,0,128,64); //border around screen
  u8g.setFontPosCenter();
  x = u8g.getStrWidth("Coin Box 1000"); //calculate centre pixel of text string
  u8g.drawStr(centreText(p,x), 10, "Coin Box 1000"); //centre x on p 
  x = u8g.getStrWidth("Insert Coins");
  u8g.drawStr(centreText(64,x), 21, "Insert Coins");
  u8g.setFont(u8g_font_lucasfont_alternate);
  u8g.setPrintPos(46,41);
  u8g.print("T:");
  u8g.setPrintPos(55,41);
  u8g.print(Temp());   
  u8g.setPrintPos(81,41);
  u8g.print("C");
  u8g.setPrintPos(46,51);
  u8g.print("H: ");
  u8g.setPrintPos(55,51);
  u8g.print(Humidity()); 
  u8g.setPrintPos(81,51);
  u8g.print("%");
  x = u8g.getStrWidth("Coins:0000 = 00.00");
  u8g.drawStr(centreText(64,x), 61, "Coins:");
  u8g.setPrintPos(51,61);
  u8g.print(coinstotal);
  u8g.setPrintPos(88,61);
  u8g.print(total/100);
}
////////////////////////////////////////////////////
void draw(void) {
  // String dollar = "$"; duplicate?
  // graphic commands to redraw the complete screen
  // Possibly function these out with the coin type insertion to speed display generation?
  u8g.setFont(u8g_font_lucasfont_alternate);
  u8g.drawFrame(0,0,128,64); //border around screen
  u8g.drawBox(2,(62-fivegraph),19,(fivegraph)); // graph draws * 6
  u8g.drawBox(23,(62-tengraph),19,(tengraph));
  u8g.drawBox(44,(62-twentygraph),19,(twentygraph));
  u8g.drawBox(65,(62-fiftygraph),19,(fiftygraph));
  u8g.drawBox(86,(62-onegraph),19,(onegraph));
  u8g.drawBox(107,(62-twograph),19,(twograph));

  float valt = total/100;
  String d = "$";
  char buffer [10];  
  String tem = dtostrf(valt, 4, 2, buffer);
  char carray[tem.length() + 1]; //determine size of the array
  tem.toCharArray(carray, sizeof(carray)); //put readStringinto an array
  Serial.print("float to string ");
  Serial.println(carray);
  String combo = d + carray;
  // String comb = dollar();  //function experiments that dont work :(   
  Serial.print("combine strings ");
  Serial.println(combo);
  char carrayy[combo.length() + 1]; //determine size of the array
  combo.toCharArray(carrayy, sizeof(carrayy)); //put readStringinto an array
  Serial.print("combo string to char ");
  Serial.println(carrayy);
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
////////////////////////////////////////////////////

/* Please note This function KILLS Arduino!////////////////////////////////////////////////////////////////////
 //this looks like the culprit char* test = dollar(total);
 
 char* dollar(float val) {
 float valt = total/100;
 String d = "$";
 char buffer [10];  
 String tem = dtostrf(valt, 4, 2, buffer);
 char carray[tem.length() + 1]; //determine size of the array
 tem.toCharArray(carray, sizeof(carray)); //put readStringinto an array
 Serial.print("float to string ");
 Serial.println(carray);
 String combo = d + carray;
 Serial.print("combine strings ");
 Serial.println(combo);
 char carrayy[combo.length() + 1]; //determine size of the array
 combo.toCharArray(carrayy, sizeof(carrayy)); //put readStringinto an array
 Serial.print("combo string to char ");
 Serial.println(carrayy);
 return carrayy;
 }
 */
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
  u8g.setPrintPos(80,55);
  u8g.print(fiftyWorth);

  //  itoa (fiftyWorth, tchar, 10);
  //  u8g.drawStr(centreText(60,x = u8g.getStrWidth(tchar)),55,tchar);
}
/////////////////////////////////////////////////////////////////
void fiftycent(void) {
  Serial.println("Fifty cent mode!");
  u8g.firstPage(); //update display
  do {
    fiftydraw();
  } 
  while( u8g.nextPage() );
  //delay(100);
  mode = 1;
  while(mode = 1) {
    delay(100);
    buttonState2 = digitalRead(buttonPin2);
    buttonState1 = digitalRead(buttonPin1);  
    if((buttonState2 == HIGH) && (buttonState1 == LOW)) {
      manualFiftyToAdd += 1;
      fiftyWorth = (manualFiftyToAdd*50)/100;
      //   char buffer[10];
      //  String tem = dtostrf(fiftyWorth, 5, 2, buffer);
      u8g.firstPage(); //update display
      do {
        fiftydraw();
      } 
      while( u8g.nextPage() );
      Serial.println("Fifty Draw Screen Complete"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)  
      delay(100);
      Serial.println(manualFiftyToAdd);
      Serial.println(fiftyWorth);
    }
    else if((buttonState1 == HIGH) && (buttonState2 == LOW)) {
      fiftyIncrement(manualFiftyToAdd);
      Serial.print("$");
      Serial.print(fiftyWorth);
      Serial.println(" added to MoneyBox");
      manualFiftyToAdd = 0;
      fiftyWorth = 0;
      Serial.println("50 Cent Amount reset to 0");
      u8g.firstPage(); //update display
      do {
        fiftydraw();
      } 
      while( u8g.nextPage() );
      Serial.println("Fifty Draw 2 Screen Complete"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)  
      delay(100);
    }
    else if((buttonState1 == HIGH) && (buttonState2 == HIGH)) { //add button 3 for value reset
      manualFiftyToAdd = 0;
      fiftyWorth = 0;
      mode = 0;
      u8g.firstPage(); //update display
      do {
        startup();
      } 
      while( u8g.nextPage() );
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
  Serial.print("Total 5 Cent Coins = ");
  Serial.println(five);

  if (EEPROM.read(3) * 256 + (EEPROM.read(4)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
    ten = 0;
    eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
  }
  else{
    ten = (EEPROM.read(3) * 256 + (EEPROM.read(4)));
    eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
  }  
  Serial.print("Total 10 Cent Coins = ");
  Serial.println(ten);

  if (EEPROM.read(3) * 256 + (EEPROM.read(4)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
    twenty = 0;
    eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
  }
  else{
    twenty = (EEPROM.read(3) * 256 + (EEPROM.read(4)));
    eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
  }  
  Serial.print("Total 20 Cent Coins = ");
  Serial.println(twenty);

  if (EEPROM.read(7) * 256 + (EEPROM.read(8)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
    fifty = 0;
    eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
  }
  else{
    fifty = (EEPROM.read(7) * 256 + (EEPROM.read(8)));
    eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
  }  
  Serial.print("Total 50 Cent Coins = ");
  Serial.println(fifty);

  if (EEPROM.read(9) * 256 + (EEPROM.read(10)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
    one = 0;
    eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
  }
  else{
    one = (EEPROM.read(9) * 256 + (EEPROM.read(10)));
    eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
  }  
  Serial.print("Total 1 Dollar Coins = ");
  Serial.println(one);

  if (EEPROM.read(11) * 256 + (EEPROM.read(12)) <= 0) { //unwritten eeprom = -1. Graphs no likey on first ever start
    two = 0;
    eepreads += 2; // monitors eeprom reads for interests sake. EEPROM limited reads/writes
  }
  else{
    two = (EEPROM.read(11) * 256 + (EEPROM.read(12)));
    eepreads += 4; //plus I love stats. may make this write to mem later. will reset between power loss
  }  
  Serial.print("Total 2 Dollar Coins = ");
  Serial.println(two);

  total = (five * 5) + (ten * 10) + (twenty * 20) + (fifty * 50) + (one * 100) + (two * 200);
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
  } 
  while( u8g.nextPage() );
  Serial.println("Draw Start Screen Complete"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)    
}

///////////////////////////////////////////////////////////////////////////////////////
void loop() {
  //char* test = dollar(total); //test function
  //Serial.println(test);
  timer += 1;
  if (timer > 50) {
    u8g.firstPage(); //update display
    do {
      startup();
    } 
    while( u8g.nextPage() );
  }
  Serial.println(timer);
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
    timer = 0;
    Serial.print("Total 5c Coins= ");
    Serial.println(five);
    EEPROM.write (1, highByte(five));
    EEPROM.write (2, lowByte(five));
  }
  else if(last == 10){
    ten += 1;
    timer = 0;
    Serial.print("Total 10c Coins= ");
    Serial.println(ten);
    EEPROM.write (3, highByte(ten));
    EEPROM.write (4, lowByte(ten));
  }
  else if(last == 20)
  {
    twenty += 1;
    timer = 0;
    Serial.print("Total 20c Coins= ");
    Serial.println(twenty);
    EEPROM.write (5, highByte(twenty));
    EEPROM.write (6, lowByte(twenty));          
  }
  else if(last == 50) { //New Increment function
    fiftyIncrement(1);
  }

  else if(last == 1)
  {
    one += 1;
    timer = 0;
    Serial.print("Total 1 Dollar Coins= ");
    Serial.println(one);
    EEPROM.write (9, highByte(one));
    EEPROM.write (10, lowByte(one));
  }
  else if(last == 2)
  {
    two += 1;
    timer = 0;
    Serial.print("Total 2 Dollar Coins= ");
    Serial.println(two);
    EEPROM.write (11, highByte(two));
    EEPROM.write (12, lowByte(two));
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
  } 
  while( u8g.nextPage() );

  float t = Temp();
  float h = Humidity();
  Serial.println("draw"); //debug to see when screen redraw (Loop of Overdraw carnage Rape)
  // Serial.println(t);
  //  Serial.println(h);

}

float Temp(void) {
  float t = dht.readTemperature();
  // Serial.print("Temperature: "); 
  //  Serial.print(t);
  //  Serial.println(" *C");
  return t;  
}

float Humidity(void) {
  float h = dht.readHumidity();
  //  Serial.print("Humidity: "); 
  //  Serial.print(h);
  //  Serial.println(" %");
  return h;
}
/////////////////////////////////////////
void fiftyIncrement(int f) { //New function for coin increment
  fifty += f;
  timer = 0;
  Serial.print("Total 50c Coins= ");
  Serial.println(fifty);
  EEPROM.write (7, highByte(fifty));
  EEPROM.write (8, lowByte(fifty));
}
/////////////////////////////////////////

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
 
 Serial.println("EEPROM Values reset to 0 for Coin Counter 1000");
 */












