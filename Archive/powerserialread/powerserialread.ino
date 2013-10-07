#define _NewSS_MAX_RX_BUFF 256
//#include <NewSoftSerial.h>
//NewSoftSerial mycc128(2, 200);
void setup() 
{
Serial.begin(9600);
Serial1.begin(57600);
}
void loop()
{
if (Serial1.available()) {
Serial.print((char)Serial1.read());
} 
}
