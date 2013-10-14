//parts from   by Bill Earl for Adafruit Industries

#include <DHT.h>

//DHT22
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

int lightPin = A2;    // select the input pin for the potentiometer
int hotPin = A0; //Hot Out
int coldPin = A1; //Cold In

float rawRange = 1024; // 3.3v
float logRange = 5.0; // 3.3v = 10^5 lux


void setup() {
  analogReference(EXTERNAL); //
  Serial.begin(9600);
  Serial.println("Adafruit Analog Light Sensor Test");
}

void loop() {
  // read the raw value from the sensor:
  int rawValue = analogRead(lightPin);    

  Serial.print("Raw = ");
  Serial.print(rawValue);
  Serial.print(" - Lux = ");
  Serial.println(RawToLux(rawValue)); 

  //read tmp
  float hotTMP = analogRead(hotPin) * 3.3; 
  //  float voltage = reading * 3.3;
  hotTMP /= 1024.0; 
  float temperatureH = (hotTMP - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((volatge - 500mV) times 100)
  Serial.print(temperatureH); 
  Serial.println(" degrees C");

  float coldTMP = analogRead(coldPin) * 3.3; 
  //  float voltage = reading * 3.3;
  coldTMP /= 1024.0; 
  float temperatureC = (coldTMP - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((volatge - 500mV) times 100)
  Serial.print(temperatureC); 
  Serial.println(" degrees C");

  Serial.print("Humidity: "); 
  Serial.println(dht.readHumidity());

  Serial.print("Humid Temperature = ");
  Serial.println(dht.readTemperature());

  delay(10000);
}

float RawToLux(int raw)
{
  float logLux = raw * logRange / rawRange;
  return pow(10, logLux);
}


