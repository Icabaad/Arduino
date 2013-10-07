#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,177);

// Enter the IP address of the server you're connecting to:
IPAddress server(119,252,190,75); //mc server ip

EthernetClient client;

void setup() {
  
Ethernet.begin(mac);
 // Open serial communications and wait for port to open:
  pinMode(13, OUTPUT);
  Serial.begin(9600); // Initialize serial port
delay(1000);
Serial.println("connecting...");

if (client.connect(server, 8712)) {
  Serial.println("connected");
    delay(1000);
    client.print(" danger"); //keep the space befor your username
    client.print("\r\n");
    client.print("m1necraft1");
    client.print("\r\n"); 
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }



}

void loop()


{
  serialReader(); 
}

void serialReader(){
  int makeSerialStringPosition;
  int inByte;
  char serialReadString[50];
  const int terminatingChar = 13; //Terminate lines with CR

  inByte = Serial.read();
  makeSerialStringPosition=0;

  if (inByte > 0 && inByte != terminatingChar) { //If we see data (inByte > 0) and that data isn't a carriage return
    delay(100); //Allow serial data time to collect (I think. All I know is it doesn't work without this.)

    while (inByte != terminatingChar && Serial.available() > 0){ // As long as EOL not found and there's more to read, keep reading
      serialReadString[makeSerialStringPosition] = inByte; // Save the data in a character array
      makeSerialStringPosition++; //Increment position in array
      //if (inByte > 0) Serial.println(inByte); // Debug line that prints the charcodes one per line for everything recieved over serial
      inByte = Serial.read(); // Read next byte
    }

    if (inByte == terminatingChar) //If we terminated properly
    {
      serialReadString[makeSerialStringPosition] = 0; //Null terminate the serialReadString (Overwrites last position char (terminating char) with 0
      Serial.println(serialReadString);
      if (strcmp(serialReadString, "LEDOn") == 0) digitalWrite(13, HIGH);
      if (strcmp(serialReadString, "LEDOff") == 0) digitalWrite(13, LOW);
    }
  } 
    if (client.available()) {
    char c = client.read();
    Serial.print(c);

  }
  
    while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (client.connected()) {
      client.print(inChar);
    }
  }
}

