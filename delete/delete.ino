#include <SPI.h>
#include "font5x7.h"

const byte LATCH = 10;
const int pin5 = 5;     // the number of the pushbutton pin
const int pin6 =  6; 
const int pin7 = 7;     // the number of the pushbutton pin
const int pin4 =  4; 

const byte CHIPS = 15;
const byte ROWS = 8;
const byte PIXELS_PER_LETTER = 5;

// what to show on the LEDs
byte bitmap [CHIPS] [ROWS];

// timer Interrupt Service Routine (ISR) to update the LEDs
ISR (TIMER2_COMPA_vect) 
  {
  static byte row = 0;
  
  digitalWrite (LATCH, LOW);
  for (byte col = 0; col < CHIPS; col++)
  SPI.transfer (bitmap [col] [row]);
 // SPI.transfer (~ (1 << row));  
  digitalWrite (LATCH, HIGH);
  // wrap if necessary
  if (++row >= ROWS)
    row = 0;
    
    for (uint8_t o=0; o < ROWS; o++) {
     PORTD |= ((o)<<4);
     for (int i=0; i < 15; i++)
	//		SPI.transfer(0);
		PORTD &= 15;
  }  // end of loop
  }  // end of TIMER2_COMPA_vect
  

// Build one letter into the bitmap.
// The variable column is updated to point past the current letter.
// Negative columns are OK, and will just skip without writing (for sideways scrolling purposes).
// Returns true once we have gone past the end of the bitmap, so the outer loop can stop.

bool outputLetter (byte c, int & column)
  {
  if (c < 0x20 || c > 0x7F)
    c = 0x7F;  // unknown glyph
  
  c -= 0x20; // force into range of our font table (which starts at 0x20)
    
  for (byte i = 0; i < PIXELS_PER_LETTER; i++)
    {
    if (column >= 0)
      {
      // work out which byte (chip) this will end up in
      int whichChip = column / 8;
      if (whichChip >= CHIPS)
        return true;
      
      // work out which column within the chip it will be in      
      byte whichColumn = column % 8;
      
      char pixels =  pgm_read_byte (&font [c] [i]);
      for (byte bit = 0; bit < ROWS; bit++)
         if (bitRead (pixels, bit))
            bitSet (bitmap [(CHIPS - 1) - whichChip] [(ROWS - 1) - bit], whichColumn);
      }  // end of column not negative
    column++;  // next row of pixels
      
    }  // end of for each of the 5 pixels in the glyph

  // gap between letters
  column++;
    
  return false;
  }  // end of outputLetter
  
// Turn a text string into a bitmap, return its length
int convertMessageToBitmap (const char * message, int column = 0)
  {
  noInterrupts ();
  memset (bitmap, 0, sizeof bitmap);
  
  for (const char * s = message; *s; s++)
    {
    if (outputLetter (*s, column))
      break;
    }  // end of for each byte in the message
  interrupts ();
  return strlen (message);
  }  // end of convertMessageToBitmap
  
void setup ()
  {
    Serial.begin(9600);
  pinMode(pin5, OUTPUT);
  pinMode(pin6, OUTPUT);
  pinMode(pin7, OUTPUT);
  pinMode(pin4, OUTPUT);


  SPI.begin ();
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  memset (bitmap, 0, sizeof bitmap);
  
  // Stop timer 2
  TCCR2A = 0;
  TCCR2B = 0;

  // Timer 2 - gives us a constant interrupt to refresh the LED display
  TCCR2A = bit (WGM21) ;   // CTC mode
  OCR2A  = 63;            // count up to 64  (zero relative!!!!)
  // Timer 2 - interrupt on match at about 2 kHz
  TIMSK2 = bit (OCIE2A);   // enable Timer2 Interrupt
  // start Timer 2
  TCCR2B =  bit (CS20) | bit (CS22) ;  // prescaler of 128
  }  // end of setup


int column;

void loop ()
  {
  static char buf [20];
  
  sprintf (buf, "Uptime %ld", millis ());
  int len = convertMessageToBitmap (buf, column--);
  
  delay (25);

  // work out when to wrap
  if (column > CHIPS * 8)
    column = - (len * (PIXELS_PER_LETTER + 1));
  else if (column < - (len * (PIXELS_PER_LETTER + 1)))
    column = CHIPS * 8;


  
  }
