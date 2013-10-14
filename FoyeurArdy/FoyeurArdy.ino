//Adafruit referenced

#include <DHT.h>

//DHT22
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

int lightPin = A2;    // Light Pin
int hotPin = A0; //Hot Out
int coldPin = A1; //Cold In

float rawRange = 1024; // 3.3v
float logRange = 5.0; // 3.3v = 10^5 lux


void setup() {
  analogReference(EXTERNAL); //
  Serial.begin(9600);
}

void loop() {
  // read the raw value from the sensor:
  int rawValue = analogRead(lightPin);    

  Serial.print("Raw = ");
  Serial.print(rawValue);
  Serial.print(" - Lux = ");
  float lightValue = (RawToLux(rawValue))
  Serial.println(lightValue); 

  //read tmp
  //float hotTMP = analogRead(hotPin) * 3.3; 
  
  float tmp36 = analogRead(hotPin) * 3.3 / 1024;
  
  //  float voltage = reading * 3.3;
  //hotTMP /= 1024.0; 
  //float temperatureH = (hotTMP - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  float hotTMP = RawToTmp(tmp36)
  Serial.print(hotTMP); 
  Serial.println(" degrees C");

  float tmp36 = analogRead(coldPin) * 3.3; 
  
  //  float voltage = reading * 3.3;
 // coldTMP /= 1024.0; 
  //float temperatureC = (coldTMP - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
  float coldTMP = RawToTmp(tmp36)
  Serial.print(coldTMP);
  Serial.println(" degrees C");

  Serial.print("Humidity: "); 
  float humidity = (dht.readHumidity())
  Serial.println(humidity);

  Serial.print("Humid Temperature = ");
  float dhTemp = (dht.readTemperature())
  Serial.println(dhTemp());

Serial.print("S");
Serial.print(lightValue);Serial.print(",");
Serial.print(hotTMP);Serial.print(",");
Serial.print(coldTMP);Serial.print(",");
Serial.print(humidity);Serial.print(",");
Serial.print(dhTemp);Serial.print(",");
  delay(5000);
  
}

float RawToLux(int raw) {
  float logLux = raw * logRange / rawRange;
  return pow(10, logLux);
}

float RawTotmp(float tmp36) {
  //float tmp = analogRead(hotPin) * 3.3 / 1024; 
  //  float voltage = reading * 3.3;
  //hotTMP /= 1024.0; 
 float tmp = (tmp - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
return tmp
}
