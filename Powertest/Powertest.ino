// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"             // Include Emon Library
EnergyMonitor emon1;             // Create an instance

float ACvoltage = 0;
float SolarW = 0;
float PPW = 0;
float HeatW = 0;
float TotalW = 0;

const int acvpin = A5;
const int solarpin = A2;
//const int analogInPin = A0
//const int analogInPin = A0

void setup()
{  
  Serial.begin(9600);

 // emon1.voltage(5, 234.26, 1.7);  // Voltage: input pin, calibration, phase_shift
 // emon1.current(1, 111.1);       // Current: input pin, calibration.
}

void loop()
{

  ACvoltage = analogRead(acvpin);
  Serial.print("AC V: ");
  Serial.println(ACvoltage);
  
    ACvoltage = analogRead(1);
  Serial.print("Solar W: ");
  Serial.println(SolarW);
  
    ACvoltage = analogRead(2);
  Serial.print("PP W: ");
  Serial.println(PPW);
  
    ACvoltage = analogRead(3);
  Serial.print("Heat W: ");
  Serial.println(HeatW);
  
    ACvoltage = analogRead(4);
  Serial.print("Total W: ");
  Serial.println(TotalW);
  delay(2000);
Serial.println();


 // emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  //emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)

 // float realPower       = emon1.realPower;        //extract Real Power into variable
 // float apparentPower   = emon1.apparentPower;    //extract Apparent Power into variable
 // float powerFActor     = emon1.powerFactor;      //extract Power Factor into Variable
 // float supplyVoltage   = emon1.Vrms;             //extract Vrms into Variable
 // float Irms            = emon1.Irms;             //extract Irms into Variable
}


