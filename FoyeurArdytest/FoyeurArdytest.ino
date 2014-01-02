//Adafruit referenced
//Awesome examples from http://www.desert-home.com/ used for XBee components <3
#include <SoftwareSerial.h>
#include <XBee.h>
#include <DHT.h>

#define rxPin 3
#define txPin 4 // again a non existing pin, compiler is OK with this...
SoftwareSerial SXbee(rxPin, txPin);

//XBEE
XBee xbee = XBee();
// This is the XBee broadcast address.  You can use the address
// of any device you have also.
XBeeAddress64 Broadcast = XBeeAddress64(0x00000000, 0x0000ffff);
char Hello[] = "Hello World\r";
char Buffer[128];  // this needs to be longer than your longest packet.

//DHT22
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT21   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

int lightPin = A2;    // Light Pin
int hotPin = A0; //Hot Out
int coldPin = A1; //Cold In

float rawRange = 1024; // 3.3v
float logRange = 5.0; // 3.3v = 10^5 lux


void setup() {
  analogReference(EXTERNAL); //
  Serial.begin(9600);
    SXbee.begin(9600);
    xbee.setSerial(SXbee);
    
    Serial.println("Starting up....");
}

void loop() {


  ZBTxRequest zbtx = ZBTxRequest(Broadcast, (uint8_t *)Hello, strlen(Hello));
  xbee.send(zbtx);
  delay(3000);
  strcpy(Buffer,"I saw what you did last night.\r");
  zbtx = ZBTxRequest(Broadcast, (uint8_t *)Buffer, strlen(Buffer));
  xbee.send(zbtx);
  delay(3000);
  Serial.println("end");
  delay(10000);
}
//********************************eND OF lOOP**********************************
float RawToLux(int raw) {
  float logLux = raw * logRange / rawRange;
  return pow(10, logLux);
}

float RawToTMP(float tmp36) {
  //float tmp = analogRead(hotPin) * 3.3 / 1024; 
  //  float voltage = reading * 3.3;
  //hotTMP /= 1024.0; 
  float tmp = (tmp36 - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  return tmp;
}


