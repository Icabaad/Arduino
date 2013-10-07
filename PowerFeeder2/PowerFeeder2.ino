// Reads data from Current Cost CC128 Power Meter and feeds it to Arduino Mega which collates with other sensors 
// and uploads to Xively and SQL

#include <SoftwareSerial.h>
#define rxPin 3
#define txPin 300 // again a non existing pin, compiler is OK with this...
SoftwareSerial currentcost(rxPin, txPin);
String readString, temp, sensor, watts;
const unsigned int MAX_INPUT = 164;
int wattsInt;
long int totalPower;
long int totalHydro = 0;
int sensorInt;
int LP;
static char input_line [MAX_INPUT];
static unsigned int input_pos = 0;
int hydroCount = 0; //hydro counter
int totalCount = 0; //power counter
int lastHydro = 0; //Last Hydro result
int lastTotal = 0; //Last total power result
void setup() {
  Serial.begin(9600);
  currentcost.begin(57600);
}

void loop() {

  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  if (currentcost.available()) {
    //  Serial.print((char)Serial1.read());
    {
      char inByte = currentcost.read ();
      switch (inByte) {
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

  if(Serial.available()) {
    char ch = Serial.read();
    switch(ch) {
    case 'S':
      Serial.print("Received Send Request");
      totalPower = totalPower / totalCount;
      if(totalPower < 1) {
        totalPower = 0;
      }
      totalHydro = totalHydro / hydroCount;
      if(totalHydro < 5) {
        totalHydro = 0;
      }

      LP = totalPower - totalHydro;
      Serial.print(totalPower,DEC); 
      Serial.print(",");  
      Serial.print(totalHydro,DEC); 
      Serial.print(",");
      Serial.print(LP,DEC);
      Serial.print(",");
      Serial.println("F");
      ch = 0;
      totalCount = 0;
      totalPower = 0;
      totalHydro = 0;
      hydroCount = 0;
      Serial.flush();
      break;
    default:
      Serial.print(ch);  
      Serial.println(" was recieved....WTF do I do with this?! ");
      Serial.flush();
      break;
    }
  }

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
    lastTotal = wattsInt;
    if(lastTotal > 10) {
      totalPower = totalPower + lastTotal;
      totalCount ++;
      //    Serial.print("Last POwer:");Serial.println (lastTotal);
      //       Serial.print("Total Count:");Serial.println (totalCount);
      //      Serial.print("Total Power:");Serial.println (totalPower);
    }
  }
  else if(sensorInt = 1) {
    lastHydro = wattsInt;
    if(lastHydro > 10) {
      totalHydro = totalHydro + lastHydro;
      hydroCount ++;
      //          Serial.print("Last hydro:");Serial.println (lastHydro);
      //        Serial.print("Hydro Count:");Serial.println (hydroCount);
      //         Serial.print("Hydro amount:");Serial.println (totalHydro);
    }
  }
  //int LP = totalPower - totalHydro;
  /* 
   if (totalHydro < 0) {
   totalHydro = 0;
   }
   if (totalPower < 0) {
   totalPower = 0;
   }
   */
}







