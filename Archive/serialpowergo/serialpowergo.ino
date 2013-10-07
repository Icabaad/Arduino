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
 // Serial.print("sensorint: ");
 // Serial.println(sensorInt);
 // Serial.print("wattsintint: ");
 // Serial.println(wattsInt);
 // Serial.print("sensor: ");
 // Serial.println(sensor);  //print to serial monitor to see parsed results
 // Serial.print("temp: ");
 // Serial.println(temp);
 // Serial.print("watts: ");
 // Serial.println(watts);
  if(sensorInt == 0) {
    totalPower = wattsInt;
  }
  else if(sensorInt = 1) {
    hydro = wattsInt;
  }
  int LP =totalPower - hydro;
 // Serial.print("totalpower: ");
  Serial.print(totalPower,DEC);
  // datastreams[9].setInt(totalPower);
 Serial.print(",");  
  Serial.print(hydro,DEC); Serial.print(","); Serial.println();  
  // datastreams[7].setInt(hydro);
 // Serial.print("LP: ");
 // Serial.println(LP);
  // datastreams[8].setInt(LP);
}

