#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Cosm.h>
#include <TSL2561.h>
#include <Wire.h>

// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Your Cosm key to let you upload data
char cosmKey[] = "1qM262XKHlzwYmJtJACB3pFV2-WSAKx1UTU5ZmVrZFMxOD0g";

//lux
TSL2561 tsl(TSL2561_ADDR_FLOAT); 
// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
int sensorPin = 0;
int motionPin = 2; // choose the input pin (for PIR sensor)
int ledPin = 3; //LED
// Define the strings for our datastream IDs
char sensorId[] = "Light";
char sensorId2[] = "Motion";
char sensorId3[] = "Lux";

CosmDatastream datastreams[] = {
  CosmDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
  CosmDatastream(sensorId2, strlen(sensorId2), DATASTREAM_FLOAT),
  CosmDatastream(sensorId3, strlen(sensorId3), DATASTREAM_FLOAT)
};

// Finally, wrap the datastreams into a feed
CosmFeed feed(74951, datastreams, 3 /* number of datastreams */);

EthernetClient client;
CosmClient cosmclient(client);
unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 5000; //delay between updates to Cosm.com

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //lux
    //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
   //tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
  
  pinMode(motionPin, INPUT);     // declare sensor as input
  pinMode(ledPin, OUTPUT);
  digitalWrite(motionPin, LOW);
  
  Serial.println("Starting multi datastream upload to Cosm...");
  Serial.println();

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(10000);
  }
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  datastreams[0].setFloat(sensorValue);
 
  
  int motion = digitalRead(motionPin);
  int PIRout = 0;
  if (motion == HIGH)
{
  digitalWrite(ledPin, HIGH);
  PIRout = 1;
}
else
{
 digitalWrite(ledPin, LOW);
   PIRout = 0;
}
datastreams[1].setFloat(PIRout);
{
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);   
   uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
datastreams[2].setFloat(full);
}
  

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    
  Serial.print("Read light sensor ");
  Serial.println(datastreams[0].getFloat());

  Serial.print("Read motion sensor ");
  Serial.println(datastreams[1].getFloat());
  
  Serial.print("Read lux sensor ");
  Serial.println(datastreams[2].getFloat());

  // Pick a random number to send up in a string
  //String stringValue(random(100));
  //stringValue += " is a random number";
  //datastreams[2].setString(stringValue);
  //Serial.print("Setting string value to:\n    ");
  //Serial.println(datastreams[2].getString());

  Serial.println("Uploading it to Cosm");
  int ret = cosmclient.put(feed, cosmKey);
  Serial.print("cosmclient.put returned ");
  Serial.println(ret);

  Serial.println();
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
  
 // Serial.print("Read sensor value ");
 // Serial.println(datastreams[0].getFloat());

 // Serial.println("Uploading it to Cosm");
  //int ret = cosmclient.put(feed, cosmKey);
  //Serial.print("cosmclient.put returned ");
  //Serial.println(ret);

  //Serial.println();
  delay(5000);
}


