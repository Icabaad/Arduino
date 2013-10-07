#include <TinyXML.h>

#define _NewSS_MAX_RX_BUFF 256
#include <SoftwareSerial.h>
//NewSoftSerial mycc128(2, 200);
#define rxPin 3
#define txPin 300 // again a non existing pin, compiler is OK with this...

// Set up the NewSoftserial port
SoftwareSerial currentcost(rxPin, txPin);

// CC128 XML status
#define XMLSTATUS_NULL   0x00  // waiting ...
#define XMLSTATUS_MSG    0x01  // got <msg>
#define XMLSTATUS_TIM    0x02  // got <msg><time>value
#define XMLSTATUS_TMP    0x03  // got <msg><tmpr>value
#define XMLSTATUS_SNSR   0x04  // got <msg><sensor>value
#define XMLSTATUS_WATT   0x05  // got <msg><ch1><watts>value

// storage
// variables
CC128Data dataSnapShot, dataCurrent;
uint8_t currentCommand;       // 0 if no command active, 0xFF if start command byte received
uint16_t electricityWatts;    // instantaneous watts (the last reading received)

// XML processing variables
TinyXML xml;
uint8_t buffer[150];
uint8_t XMLstatus;
char XMLtime[9];
char XMLtmpr[5];
char XMLsensor;
char XMLwatts[6];

// Used by timeout code - used to detect a protocol lockup 
// (e.g. we didn't sucessfully receive an end tag and so never see the end of a message)
// can happen when other CPU demands cause Serial buffer overflow.
// assumption here is that we should receive three msgs every 6 seconds 
// - so if x seconds have elapsed after the last message then TinyXML needs a reset
#define XML_TIMEOUT 15000      // 15 seconds
uint32_t timeout_millis;      // holds the time of the last received msg

// start of code

void setup()
{
   pinMode(13,OUTPUT);
  memset(&dataCurrent,0,sizeof(CC128Data));  // zero out the structure
  memset(&dataSnapShot,0,sizeof(CC128Data)); // zero out the structure
   xml.init((uint8_t*)&buffer,sizeof(buffer),&XML_callback);
  currentcost.begin(57600);
}

void loop()
{
  while (Serial.available())
    xml.processChar(Serial.read());

  //timeout code
  if (millis()-timeout_millis > XML_TIMEOUT)
  {
    xml.reset();
    dataCurrent.timeout_count++;
    timeout_millis=millis();  // don't re-trigger for another XML_TIMEOUT
  }
}

// CC128 call back events
void XML_callback( uint8_t statusflags, char* tagName,  uint16_t tagNameLen,  char* data,  uint16_t dataLen )
{
  if  (statusflags & STATUS_ERROR)
  {
    XMLstatus = XMLSTATUS_NULL;
    dataCurrent.errorCount++;
  }
  else if ((statusflags & STATUS_END_TAG) && (tagNameLen == 4) && (XMLstatus!=XMLSTATUS_WATT))
  {
    if ( strcmp(tagName,"/msg") == 0 )
    {
      //we have a message that didn't have all the data (e.g. history)
      XMLstatus = XMLSTATUS_NULL; // just set to null status
      //Serial.println("History msg");
    }
  }
  switch (XMLstatus)
  {
  case XMLSTATUS_NULL:
    if (statusflags & STATUS_START_TAG)
    {
      if ( tagNameLen )
      {
        if ( strcmp(tagName,"/msg") == 0 ) XMLstatus = XMLSTATUS_MSG;
      }
    }
    break;
  case XMLSTATUS_MSG:
    if  (statusflags & STATUS_TAG_TEXT)
    {
      if ( strcmp(tagName,"/msg/time") == 0 )
      {
        strncpy(XMLtime,data,8);
        XMLtime[8]=0;
        XMLstatus = XMLSTATUS_TIM;
      }
    }
    break;
  case XMLSTATUS_TIM:
    if  (statusflags & STATUS_TAG_TEXT)
    {
      if ( strcmp(tagName,"/msg/tmpr") == 0 )
      {
        strncpy(XMLtmpr,data,4);
        XMLtmpr[4]=0;
        XMLstatus = XMLSTATUS_TMP;
      }
    }
    break;
  case XMLSTATUS_TMP:
    if  (statusflags & STATUS_TAG_TEXT)
    {
      if ( strcmp(tagName,"/msg/sensor") == 0 )
      {
        XMLsensor = data[0];
        XMLstatus = XMLSTATUS_SNSR;
      }
    }
    break;
  case XMLSTATUS_SNSR:
    if  (statusflags & STATUS_TAG_TEXT)
    {
      if ( strcmp(tagName,"/msg/ch1/watts") == 0 )
      {
        strncpy(XMLwatts,data,5);
        XMLwatts[5]=0;
        XMLstatus = XMLSTATUS_WATT;
      }
    }
    break;
  case XMLSTATUS_WATT:
    if  (statusflags & STATUS_END_TAG)
    {
      if ( strcmp(tagName,"/msg") == 0 )
      {
        XMLstatus = XMLSTATUS_NULL;
        processMsg();
      }
    }
    break;
  }
}

//
// got a CC128 <msg> so process it
//
void processMsg()
{
  timeout_millis = millis();      // reset timeout
  digitalWrite(13,HIGH);

  dataCurrent.lastTimeReceived=((uint32_t)atoi(&XMLtime[0])*60+(uint32_t)atoi(&XMLtime[3]))*60+(uint32_t)atoi(&XMLtime[6]);
  dataCurrent.temperature=uint16_t(atof(&XMLtmpr[0])*10);
  switch (XMLsensor)
  {
  case '0':    // Electricity
    electricityWatts = atoi(&XMLwatts[0]);
    dataCurrent.elecWatts += electricityWatts;
    dataCurrent.elecSamples++;
    break;
  case '1':    // Gas
    if (atoi(&XMLwatts[0])) dataCurrent.gasCounts++;
    dataCurrent.gasSamples++;
    break;
  case '2':    // Light
    dataCurrent.lightValue = atoi(&XMLwatts[0]);
    break;
  }
  digitalWrite(13,LOW);
}

