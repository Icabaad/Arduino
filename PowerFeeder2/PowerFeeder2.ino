#include <EmonLib.h>

#include <SoftwareSerial.h>
#define rxPin 3
#define txPin 300 // again a non existing pin, compiler is OK with this...
SoftwareSerial currentcost(rxPin, txPin);
String readString, temp, sensor, watts;
const unsigned int MAX_INPUT = 164;
int wattsInt;
int totalPower;
int hydro;
int sensorInt;
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;
int hydroCount = 0; //hydro counter
int powerCount = 0; //power counter
int lastTotal = 0; //Last Hydro result
int lastTotal = 0; //Last total power result
void setup() {
  Serial.begin(9600);

  currentcost.begin(57600);
}



void loop() {

  //   delay(500);


  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  if (currentcost.available()) {
    // delay(1);
    //***********************    
    //  Serial.print((char)Serial1.read());
    {
      char inByte = currentcost.read ();

      switch (inByte)
      {
      case '\n':   // end of text
        input_line [input_pos] = 0;  // terminating null byte

        // terminator reached! process input_line here ...
        process_data (input_line);

        // reset buffer for next time
        input_pos = 0;  
        break;

      case '\r':   // discard carriage return
        break;

      default:
        // keep adding if not full ... allow for terminating null byte
        if (input_pos < (MAX_INPUT - 1))
          input_line [input_pos++] = inByte;
   //     Serial.print(inByte);
        break;

      }  // end of switch

    }  // end of incoming data

  }
  //********************Experimental Send Recieve code**************************
  
  if(Serial.available() {
  char ch = Serial.read();
  switch(ch) {
    case 'S':
      Serial.print(totalPower,DEC); Serial.print(",");  
  Serial.print(hydro,DEC); Serial.print(",");
  Serial.print(LP,DEC);Serial.print(",");Serial.print("F");
    */
}

void process_data (char * data)
{
  // for now just display it
  //Serial.println (data);
  readString = data; //makes the string readString
  temp = readString.substring(70, 74); //get the first four characters
  sensor = readString.substring(89, 90); //get the next four characters 
  watts = readString.substring(139, 144);
  wattsInt = watts.toInt();  //convert readString into a number
  sensorInt = sensor.toInt();

  if(sensorInt == 0) {
    lastPower = wattsInt
    totalPower + lastpower;
    totalCount ++;
    Serial.print("Total Count:");Serial.println (totalCount);
  }
  else if(sensorInt = 1) {
    lastHydro = wattsInt
    hydro + wattsInt;
    hydroCount ++;
    Serial.print("Hydro Count:");Serial.println (hydroCount);
  }
  int LP =totalPower - hydro;
  
  if (hydro < 0) {
    hydro = 0;
  }
  if (totalPower < 0) {
    totalPower = 0;
  }
 // Serial.print("totalpower: ");
 
 
 Serial.print("S");
  Serial.print(totalPower,DEC); Serial.print(",");  
  Serial.print(hydro,DEC); Serial.print(",");
  Serial.print(LP,DEC);Serial.print(",");Serial.print("F");
  Serial.println();  
 // Serial.print("LP: ");
// Serial.println(LP);
  // datastreams[8].setInt(LP);
}

