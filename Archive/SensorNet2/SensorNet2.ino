// Sensor Mega! V.001
#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <TSL2561.h>
#include <XBee.h>

//BMP085 Pressure
Adafruit_BMP085 bmp;
//DHT22
#define DHTPIN 5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
//lux
TSL2561 tsl(TSL2561_ADDR_LOW);
//xbee
XBee xbee = XBee();
ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();
XBeeAddress64 test = XBeeAddress64();

//ethernet
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Your Xively key to let you upload data
char xivelyKey[] = "8q9hxZHPQggWKsXqPEBZymEHa5Uyfiwv1TrcPXBenKPCsCr7";

// Define the strings for our datastream IDs
char sensorId0[] = "Comms_Motion";
char sensorId1[] = "Comms_Temp";
char sensorId2[] = "Comms_Barometer";
char sensorId3[] = "Comms_Humidity";
char sensorId4[] = "Comms_Lux";
char sensorId5[] = "Outdoor_Temp";

//char bufferId[] = "info_message";
//String stringId("random_string");
const int bufferSize = 140;
char bufferValue[bufferSize]; // enough space to store the string we're going to send

XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId0, strlen(sensorId0), DATASTREAM_INT),
    XivelyDatastream(sensorId1, strlen(sensorId1), DATASTREAM_FLOAT),
    XivelyDatastream(sensorId2, strlen(sensorId2), DATASTREAM_FLOAT),
    XivelyDatastream(sensorId3, strlen(sensorId3), DATASTREAM_FLOAT),
    XivelyDatastream(sensorId4, strlen(sensorId4), DATASTREAM_INT),
    XivelyDatastream(sensorId5, strlen(sensorId3), DATASTREAM_FLOAT),    
    
 // XivelyDatastream(bufferId, strlen(bufferId), DATASTREAM_BUFFER, bufferValue, bufferSize),
 // XivelyDatastream(stringId, DATASTREAM_STRING)
  };
  // Finally, wrap the datastreams into a feed
  XivelyFeed feed(1177751918, datastreams, 5 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

int sensorPin = 0; //light
int motionPin = 2; // choose the input pin (for PIR sensor)
int ledPin = 3; //LED



void setup() {
  xbee.begin(9600);
  Serial.begin(9600);
  Serial.println("Starting multiple datastream upload to Xively...");
  Serial.println();

  //Ethernet OK?
  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
    Serial.print("My IP address: ");
    Ethernet.localIP().printTo(Serial);
    Serial.println();

    Serial.print("Gateway IP address is ");
    Ethernet.gatewayIP().printTo(Serial);
    Serial.println();

    Serial.print("DNS IP address is ");
    Ethernet.dnsServerIP().printTo(Serial);
    Serial.println();
    
  Serial.println("Ethernet OK....");
  //Barometer OK?
  //time for barometer to start up
  Serial.println("Barometer Warmup Phase..."); 
  delay(2000); 
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {
    }
  }
  //Lux  
  //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain for dim situations
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
  
  Serial.println("Barometer OK.....");

  pinMode(motionPin, INPUT);     // declare sensor as input
  pinMode(ledPin, OUTPUT);
  digitalWrite(motionPin, LOW);

}


void loop() {
  Serial.print("Motion Sensor State: "); 
  int commsMotion = digitalRead(motionPin);
  int pirOut = 0;
  Serial.println(commsMotion);
  if (commsMotion == HIGH) {
    digitalWrite(ledPin, HIGH);
    commsMotion = 1;
  }
  else {
    digitalWrite(ledPin, LOW);
    commsMotion = 0;
  }
  datastreams[0].setInt(commsMotion);
  Serial.print("Read Comms motion value ");
  Serial.println(datastreams[0].getInt());

  float barometerTemp = (bmp.readTemperature());
  Serial.print("Barometer Temperature = ");
  Serial.print(barometerTemp);
  Serial.println(" *C");
  datastreams[1].setFloat(barometerTemp);
  
  float barometerPressure = (bmp.readPressure());
  Serial.print("Pressure = ");
  Serial.print(barometerPressure);
  Serial.println(" Pa");
  datastreams[2].setFloat(barometerPressure);
 
  Serial.print("Humidity: "); 
  Serial.print(dht.readHumidity());
  Serial.println(" %\t");
   datastreams[3].setFloat(dht.readHumidity());
  
  Serial.print("Humid Temperature = ");
  Serial.print(dht.readTemperature());
  Serial.println(" *C Not Logged");
 
 
 //Luminosity Full Visible calcs
   Serial.print("Full LUX = ");
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);   
   uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.println(full);
  datastreams[4].setInt(full);
  
  //xbee
    //attempt to read a packet    
  xbee.readPacket();

  if (xbee.getResponse().isAvailable()) {
     if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
      xbee.getResponse().getZBRxIoSampleResponse(ioSample);
      if (ioSample.containsAnalog()) {
        Serial.println("Sample contains analog data");
      }
      if (ioSample.containsDigital()) {
        Serial.println("Sample contains digtal data");
      }      
      // read analog inputs
      for (int i = 0; i <= 4; i++) {
        if (ioSample.isAnalogEnabled(i)) {
          Serial.print("Analog (AI");
          Serial.print(i, DEC);
          Serial.print(") is ");

          int reading = (ioSample.getAnalog(i));
          float voltage = reading * 1.2;
          voltage /= 1024.0; 
        
 float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((volatge - 500mV) times 100)
 Serial.print(temperatureC); Serial.println(" degrees C");
   datastreams[5].setFloat(temperatureC);
        }
      }
     }
  }
  /*
  datastreams[1].setBuffer("Comms Motion");
  Serial.print("Setting buffer value to:\n    ");
  Serial.println(datastreams[1].getBuffer());

  // Pick a random number to send up in a string
  String stringValue(random(100));
  stringValue += " is a random number";
  datastreams[2].setString(stringValue);
  Serial.print("Setting string value to:\n    ");
  Serial.println(datastreams[2].getString());
*/

  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);

  Serial.println();
  
  //SQL feed
 // Serial.print("SQL:");Serial.print(barometerTemp);Serial.print(",");Serial.println(dht.readHumidity());
  delay(60000);
}





