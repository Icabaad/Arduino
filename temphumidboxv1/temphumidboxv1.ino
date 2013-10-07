/*
The circuit:
* LCD RS pin to digital pin 8
* LCD Enable pin to digital pin 9
* LCD D4 pin to digital pin 4
* LCD D5 pin to digital pin 5
* LCD D6 pin to digital pin 6
* LCD D7 pin to digital pin 7
* LCD R/W pin to ground
* 10K resistor:
* ends to +5V and ground
* wiper to LCD VO pin (pin 3)

*/

// include the library code:
#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int ButtonVoltage = 0;
int ButtonPressed = 0;
int Backlight = 10;
int fadeValue = 255;

#define BUTTON_SELECT    5
#define BUTTON_LEFT      4
#define BUTTON_DOWN      3
#define BUTTON_UP        2
#define BUTTON_RIGHT     1
#define BUTTON_NONE      0


void printnum(int n) {
    if (n<1000) {
        lcd.print(" ");
        if (n<100) {
            lcd.print(" ");
            if (n<10) {
                lcd.print(" ");
            }
        }
    }
    lcd.print(n);

}
byte armsDown[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010
};
byte armsUp[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};

void setup() {
    // create a new character
  lcd.createChar(3, armsDown);  
  // create a new character
  lcd.createChar(4, armsUp);  
  
// set up the LCD's number of columns and rows: 
lcd.begin(16, 2);
// Print a message to the LCD.
lcd.setCursor (2,0);
lcd.print("Initializing");
lcd.setCursor (1,1);
lcd.print("DangerTemp 1.0");
delay(5000);
lcd.clear();
lcd.setCursor (0,0);
lcd.print("  Suppressing");
lcd.setCursor (0,1);
lcd.print("    Ben Hate");
lcd.setCursor (12,1);
delay(900);
lcd.print(".");
delay(2000);
lcd.print(".");
delay(2000);
lcd.clear();
lcd.print("This is Madness!");
delay(2000);
lcd.clear();

lcd.setCursor (0,0);
lcd.print("Temp: ");
lcd.setCursor (0,1);
lcd.print("Humidity: ");
lcd.setCursor (15,1);
lcd.print("%");

dht.begin();
}

void loop() {
// set the cursor to column 0, line 1
// (note: line 1 is the second row, since counting begins with 0):
//lcd.setCursor(12, 1);
//lcd.print (millis()/1000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

lcd.setCursor(10, 0);
lcd.print (t);
lcd.setCursor(10, 1);
lcd.print (h);


  // read the potentiometer on A0:
  int sensorReading = (t);
  // map the result to 200 - 1000:
  int delayTime = map(sensorReading, 10, 40, 1000, 10);
  // set the cursor to the bottom row, 5th position:
  lcd.setCursor(15, 0);
  // draw the little man, arms down:
  lcd.write(3);
  delay(delayTime);
  lcd.setCursor(15, 0);
  // draw him arms up:
  lcd.write(4);
  delay(delayTime); 

ButtonVoltage = analogRead(0);

lcd.setCursor (4,0);
// Observed values:
//     NONE:    1023
//     SELECT:  723
//     LEFT:    481
//     DOWN:    307
//     UP:      133
//     RIGHT:   0

if (ButtonVoltage > 800) ButtonPressed = BUTTON_NONE;    // No button pressed should be 1023
else if (ButtonVoltage > 500) ButtonPressed = BUTTON_SELECT;   
else if (ButtonVoltage > 400) ButtonPressed = BUTTON_LEFT;   
else if (ButtonVoltage > 250) ButtonPressed = BUTTON_DOWN;   
else if (ButtonVoltage > 100) ButtonPressed = BUTTON_UP; 
else ButtonPressed = BUTTON_RIGHT;

switch (ButtonPressed) {
case BUTTON_SELECT:
            lcd.print ("Select");
            break;
case BUTTON_LEFT:
            lcd.print ("Left  ");
            break;
case BUTTON_DOWN:     {
                lcd.print ("Down  ");
                fadeValue = fadeValue -5;
                if (fadeValue < 5) { fadeValue = 0; }
                analogWrite (Backlight, fadeValue);
                lcd.setCursor(12,0);
                printnum(fadeValue);
                delay (100);
            }
            break;
case BUTTON_UP:     {
                lcd.print ("Up    ");
                fadeValue = fadeValue +5;
                if (fadeValue > 254) { fadeValue = 255; }
                analogWrite (Backlight, fadeValue);
                lcd.setCursor(12,0);
                printnum(fadeValue);
                delay (100);
            }
            break;
case BUTTON_RIGHT:
            lcd.print ("Right ");
            break;
case BUTTON_NONE:
//            lcd.print ("None  ");
            break;
}


// print the number of seconds since reset:
//lcd.setCursor(0,1);
//lcd.print("V:");

//printnum(ButtonVoltage);
}

