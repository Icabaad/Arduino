/***********************************************************************
 * demo16x24.c - Arduino demo program for Holtek HT1632 LED driver chip,
 *            As implemented on the Sure Electronics DE-DP016 display board
 *            (16*24 dot matrix LED module.)
 * Nov, 2008 by Bill Westfield ("WestfW")
 *   Copyrighted and distributed under the terms of the Berkeley license
 *   (copy freely, but include this notice of original author.)
 *
 * Dec 2010, FlorinC - adapted for 3216 display
 * March 2011, kjus - speed improvements:
 *   - Update only necessary leds (i.e. update red and green leds separately).
 *   - Use faster version of write functions.
 * Based from code HT1638_7.zip from:
 * http://forums.parallax.com/showthread.php?128037-Sure-Electronics-new-32x16-bi-color-display-3216-RG
 ***********************************************************************/

#include <WProgram.h>
#include "ht1632.h"
#include <avr/pgmspace.h>
#include "font.h"
#include "font2.h"
#include "font3.h"
#include "DigitalWriteFast.h" 

#define Number_of_Displays 1
#define CHIP_MAX (4*Number_of_Displays) //Four HT1632Cs on one board
#define X_MAX (32*Number_of_Displays -1)
#define Y_MAX 15

#define CLK_DELAY

// If defined, fps of the screen will be computed when calling compute_fps()
// and sent from time to time to the serial interface.
#define COMPUTE_FPS

// possible values for a pixel;
//RANDOMCOLOR for each line a different color
//MULTICOLOR for each point a different random color
#define BLACK  0
#define GREEN  1
#define RED    2
#define ORANGE 3
#define RANDOMCOLOR 4 // only for scrolling functions
#define MULTICOLOR 5

// insert here the number of columns of your font files 
// the compiler will comment how large the number of columns
// should be
#define NCOLUMNS 8

#define LONGDELAY 1000  // This delay BETWEEN demos


#define plot(x,y,v)  ht1632_plot(x,y,v)
#define cls          ht1632_clear



// our own copy of the "video" memory; 64 bytes for each of the 4 screen quarters;
// each 64-element array maps 2 planes:
// indexes from 0 to 31 are allocated for green plane;
// indexes from 32 to 63 are allocated for red plane;
// when a bit is 1 in both planes, it is displayed as orange (green + red);
byte ht1632_shadowram[64][4*Number_of_Displays] = {0};



/*
 * Set these constants to the values of the pins connected to the SureElectronics Module
 */
static const byte ht1632_data = 6;  // Data pin (pin 7 of display connector)
static const byte ht1632_wrclk = 7; // Write clock pin (pin 5 of display connector)
static const byte ht1632_cs = 8;    // Chip Select (pin 1 of display connnector)
static const byte ht1632_clk = 9; // clock pin (pin 2 of display connector)


//**************************************************************************************************
//Function Name: OutputCLK_Pulse
//Function Feature: enable CLK_74164 pin to output a clock pulse
//Input Argument: void
//Output Argument: void
//**************************************************************************************************
void OutputCLK_Pulse(void) //Output a clock pulse
{
  digitalWriteFastNoCheck(ht1632_clk, HIGH);
  digitalWriteFastNoCheck(ht1632_clk, LOW);
}


//**************************************************************************************************
//Function Name: OutputA_74164
//Function Feature: enable pin A of 74164 to output 0 or 1
//Input Argument: x: if x=1, 74164 outputs high. If x?1, 74164 outputs low.
//Output Argument: void
//**************************************************************************************************
void OutputA_74164(unsigned char x) //Input a digital level to 74164
{
  digitalWriteFastNoCheck(ht1632_cs, (x==1 ? HIGH : LOW));
}


//**************************************************************************************************
//Function Name: ChipSelect
//Function Feature: enable HT1632C
//Input Argument: select: HT1632C to be selected
// If select=0, select none.
// If s<0, select all.
//Output Argument: void
//**************************************************************************************************
void ChipSelect(int select)
{
  unsigned char tmp = 0;
  if(select<0) //Enable all HT1632Cs
  {
    OutputA_74164(0);
    CLK_DELAY;
    for(tmp=0; tmp<CHIP_MAX; tmp++)
    {
      OutputCLK_Pulse();
    }
  }
  else if(select==0) //Disable all HT1632Cs
  {
    OutputA_74164(1);
    CLK_DELAY;
    for(tmp=0; tmp<CHIP_MAX; tmp++)
    {
      OutputCLK_Pulse();
    }
  }
  else
  {
    OutputA_74164(1);
    CLK_DELAY;
    for(tmp=0; tmp<CHIP_MAX; tmp++)
    {
      OutputCLK_Pulse();
    }
    OutputA_74164(0);
    CLK_DELAY;
    OutputCLK_Pulse();
    CLK_DELAY;
    OutputA_74164(1);
    CLK_DELAY;
    tmp = 1;
    for( ; tmp<select; tmp++)
    {
      OutputCLK_Pulse();
    }
  }
}


/*
 * ht1632_writebits
 * Write bits (up to 8) to h1632 on pins ht1632_data, ht1632_wrclk
 * Chip is assumed to already be chip-selected
 * Bits are shifted out from MSB to LSB, with the first bit sent
 * being (bits & firstbit), shifted till firsbit is zero.
 */
void ht1632_writebits (byte bits, byte firstbit)
{
  DEBUGPRINT(" ");
  while (firstbit) {
    DEBUGPRINT((bits&firstbit ? "1" : "0"));
    digitalWriteFastNoCheck(ht1632_wrclk, LOW);
    if (bits & firstbit) {
      digitalWriteFastNoCheck(ht1632_data, HIGH);
    } 
    else {
      digitalWriteFastNoCheck(ht1632_data, LOW);
    }
    digitalWriteFastNoCheck(ht1632_wrclk, HIGH);
    firstbit >>= 1;
  }
}


/*
 * ht1632_sendcmd
 * Send a command to the ht1632 chip.
 */
static void ht1632_sendcmd (byte chipNo, byte command)
{
  ChipSelect(chipNo);
  ht1632_writebits(HT1632_ID_CMD, 1<<2);  // send 3 bits of id: COMMMAND
  ht1632_writebits(command, 1<<7);  // send the actual command
  ht1632_writebits(0, 1); 	/* one extra dont-care bit in commands. */
  ChipSelect(0);
}


/*
 * ht1632_senddata
 * send a nibble (4 bits) of data to a particular memory location of the
 * ht1632.  The command has 3 bit ID, 7 bits of address, and 4 bits of data.
 *    Select 1 0 1 A6 A5 A4 A3 A2 A1 A0 D0 D1 D2 D3 Free
 * Note that the address is sent MSB first, while the data is sent LSB first!
 * This means that somewhere a bit reversal will have to be done to get
 * zero-based addressing of words and dots within words.
 */
static void ht1632_senddata (byte chipNo, byte address, byte data)
{
  ChipSelect(chipNo);
  ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
  ht1632_writebits(address, 1<<6); // Send address
  ht1632_writebits(data, 1<<3); // send 4 bits of data
}


void ht1632_setup()
{
  pinModeFast2(ht1632_cs, OUTPUT);
  digitalWriteFastNoCheck(ht1632_cs, HIGH); 	/* unselect (active low) */
  pinModeFast2(ht1632_wrclk, OUTPUT);
  pinModeFast2(ht1632_data, OUTPUT);
  pinModeFast2(ht1632_clk, OUTPUT);

  for (int j=1; j<=CHIP_MAX; j++)
  {
    ht1632_sendcmd(j, HT1632_CMD_SYSDIS);  // Disable system
    ht1632_sendcmd(j, HT1632_CMD_COMS00);
    ht1632_sendcmd(j, HT1632_CMD_MSTMD); 	/* Master Mode */
    ht1632_sendcmd(j, HT1632_CMD_RCCLK);  // HT1632C
    ht1632_sendcmd(j, HT1632_CMD_SYSON); 	/* System on */
    ht1632_sendcmd(j, HT1632_CMD_LEDON); 	/* LEDs on */
  
  for (byte i=0; i<96; i++)
  {
    ht1632_senddata(j, i, 0);  // clear the display!
  }
  }
}

//**************************************************************************************************
//Function Name: xyToIndex
//Function Feature: get the value of x,y
//Input Argument: x: X coordinate
//                y: Y coordinate
//Output Argument: address of xy
//**************************************************************************************************
byte xyToIndex(byte x, byte y)
{
  byte addr;

  x = x % 16;
  y = y % 8;
  addr = (x<<1) + (y>>2); // (x % 16) * 2 + (y % 8) / 4

  return addr;
}

//**************************************************************************************************
//Function Name: calcBit
//Function Feature: calculate the bitval of y
//Input Argument: y: Y coordinate
//Output Argument: bitval
//**************************************************************************************************
#define calcBit(y) (8>>(y&3))

// Return the (1-based) index of the HT1632C chip that should be used for the given coordinates.
byte ht1632_get_chip(int x, int y) {
  if (x>=96) {
    return 7 + x/16 + (y>7?2:0);
  } 
  else if (x>=64) {
    return 5 + x/16 + (y>7?2:0);
  }    
   else if (x>=32) {
    return 3 + x/16 + (y>7?2:0);
  }  
  else {
    return 1 + x/16 + (y>7?2:0);
  } 
}

//**************************************************************************************************
//Function Name: get_pixel
//Function Feature: get the value of x,y
//Input Argument: x: X coordinate
//                y: Y coordinate
//Output Argument: color set on x,y coordinates
//**************************************************************************************************
int get_pixel(byte x, byte y) {
  byte addr, bitval, nChip;
  
  nChip = ht1632_get_chip(x, y);
  addr = xyToIndex(x,y);
  bitval = calcBit(y);

  if ((ht1632_shadowram[addr][nChip-1] & bitval) && (ht1632_shadowram[addr+32][nChip-1] & bitval)) {
    return ORANGE;
  } else if (ht1632_shadowram[addr][nChip-1] & bitval) {
    return GREEN;
  } else if (ht1632_shadowram[addr+32][nChip-1] & bitval) {
    return RED;
  } else {
    return BLACK;
  }
}

// Updates the shadowram at the given address for the given chip.
// pixel_bitval should contain a single binary '1' at the place corresponding
// to the current pixel, and target_bitval should contain the actual target
// value of that bit.
// Returns true if the screen needs actual updating.
boolean ht1632_update_shadowram(byte addr,
				byte nChip,
				byte target_bitval,
				byte pixel_bitval) {
  byte& v = ht1632_shadowram[addr][nChip-1];
  if ((v ^ target_bitval) & pixel_bitval) {
    // We need to modify the actual displayed value.
    if (target_bitval) {
      v |= pixel_bitval;
    } else {
      v &= ~pixel_bitval;   
    }
    return true;
  }
  return false;
}

/*
 * plot a point on the display, with the upper left hand corner
 * being (0,0), and the lower right hand corner being (X_MAX, Y_MAX);
 * parameter "color" could have one of the 4 values:
 * black (off), red, green or yellow;
 */
void ht1632_plot (int x, int y, byte color)
{
   byte nChip, addr, bitval;
  
  if (x<0 || x>X_MAX || y<0 || y>Y_MAX)
    return;
  
  if (color != BLACK && color != GREEN && color != RED && color != ORANGE)
    return;  

  nChip = ht1632_get_chip(x, y);
  addr = xyToIndex(x,y);
  bitval = calcBit(y);
  
  // Only modify the value of the pixel in the red and green planes if
  // necessary.
  byte red = 0, green = 0; // target bitvals for the red and green planes.
  switch (color) {
    case GREEN:
      green = bitval;
      break;
    case RED:
      red = bitval;
      break;
    case ORANGE:
      green = red = bitval;
      break;
  }
  if (ht1632_update_shadowram(addr, nChip, green, bitval)) {
    ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
  }
  addr += 32;
  if (ht1632_update_shadowram(addr, nChip, red, bitval)) {
    ht1632_senddata(nChip, addr, ht1632_shadowram[addr][nChip-1]);
  }
}


/*
 * ht1632_clear
 * clear the display, and the shadow memory, and the snapshot
 * memory.  This uses the "write multiple words" capability of
 * the chipset by writing all 96 words of memory without raising
 * the chipselect signal.
 */
void ht1632_clear()
{
  char i;
  for (int i=1; i<=8; i++)
  {
    ChipSelect(-1);
    ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
    ht1632_writebits(0, 1<<6); // Send address
    for (i = 0; i < 96/2; i++) // Clear entire display
      ht1632_writebits(0, 1<<7); // send 8 bits of data
    ChipSelect(0);

    for (int j=0; j < 64; j++)
      ht1632_shadowram[j][i] = 0;
  }
}


void ht1632_putchar(int x, int y, char c, byte color=GREEN)
{
  byte dots;
  //if (c >= 'A' && c <= 'Z' ||
  //  (c >= 'a' && c <= 'z') ) {
  //  c &= 0x1F;   // A-Z maps to 1-26
  //} 
  //else if (c >= '0' && c <= '9') {
  //  c = (c - '0') + 27;
  //} 
  //else if (c == ' ') {
  //  c = 0; // space
  //}
  
  if (c == ' ') {c = 0;}
  else if (c == '!') {c = 1;}
  else if (c == '"') {c = 2;}
  else if (c == '#') {c = 3;}
  else if (c == '$') {c = 4;}
  else if (c == '%') {c = 5;}
  else if (c == '&') {c = 6;}
  //else if (c == ''') {c = 7;}
  else if (c == '(') {c = 8;}
  else if (c == ')') {c = 9;}
  else if (c == '*') {c = 10;}
  else if (c == '+') {c = 11;}
  else if (c == ',') {c = 12;}
  else if (c == '-') {c = 13;}
  else if (c == '.') {c = 14;}
  else if (c == '/') {c = 15;}  
  
  else if (c >= '0' && c <= '9') {
    c = (c - '0') + 16;
  }
  
  else if (c == ':') {c = 26;} 
  else if (c == ';') {c = 27;} 
  else if (c == '<') {c = 28;}
  else if (c == '=') {c = 29;}
  else if (c == '>') {c = 30;}
  else if (c == '?') {c = 31;}
  else if (c == '@') {c = 32;}   
  
  else if (c >= 'A' && c <= 'Z') {
    c = (c - 'A') + 33;
  }
  
  else if (c == '[') {c = 59;}
  //else if (c == '\') {c = 60;}
  else if (c == ']') {c = 61;}
  else if (c == '^') {c = 62;}
  else if (c == '_') {c = 63;}
  else if (c == '`') {c = 64;}
  
  else if (c >= 'a' && c <= 'z') {
    c = (c - 'a') + 65;
  }
  
  else if (c == '{') {c = 91;}
  else if (c == '|') {c = 92;}
  else if (c == '}') {c = 93;}
  
  for (char col=0; col< 6; col++) {
    dots = pgm_read_byte_near(&my3font[c][col]);
    for (char row=0; row <=7; row++) {
      if (dots & (64>>row))   	     // only 7 rows.
        plot(x+col, y+row, color);
      else 
        plot(x+col, y+row, 0);
    }
  }
}










/*
 * Copy a character glyph from the myfont data structure to
 * display memory, with its upper left at the given coordinate
 * This is unoptimized and simply uses plot() to draw each dot.
 * Slightly adopted for using fonts with more rows then 8
 *  ht1632_putcharsizecolor(x location, y location , char ,  size as integer, colorname (RANDOMCOLOR for random color),  name of the font array,  umber of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory font arrays) 
 */
void ht1632_putcharsizecolor(int x, int y,unsigned char c,  char size, byte color, byte secondcolor, unsigned char fontname[][NCOLUMNS],  int columncountfont, char rowcountfont, char oddeven)
{

  byte dots, dots2,dots3, cc,cc2, cc3, rr, g, t, t3, divisor;
  byte maximumdrawfont, showcolor,showsecondcolor; //128 for the large fonts (=8x8 and 12x8), 64 for all smaller ones
  if  (rowcountfont<=7)
    maximumdrawfont=64;
  else
    maximumdrawfont=128;
  for (byte col=0; col<columncountfont*size ; col++) {
    // Addressing the right 8 lines because 'The Dot Factory' starts from the bottom, all others from top  
    if (rowcountfont <=8) {
      cc=col/size;
      dots = pgm_read_byte_near(&fontname[c][cc]);
      divisor=1;
    } 
    else if (rowcountfont>8 && rowcountfont <=16){
      if (oddeven=='T'){
        g=0;
        t=1;
      }
      else {
        g=1;
        t=0;
      }

      cc=col/size*2+g;
      cc2=col/size*2+t;
      dots = pgm_read_byte_near(&fontname[c][cc]);
      dots2 = pgm_read_byte_near(&fontname[c][cc2]);  
      divisor=2;
    }
    else if (rowcountfont>16 && rowcountfont <=24){
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
      }
      else {
        g=2;
        t=1;
        t3=0;
      }
      divisor=3;
      cc=col/size*divisor+g;
      cc2=col/size*divisor+t;
      cc3=col/size*divisor+t3;
      dots = pgm_read_byte_near(&fontname[c][cc]);
      dots2 = pgm_read_byte_near(&fontname[c][cc2]);  
      dots3 = pgm_read_byte_near(&fontname[c][cc3]); 

    }
    for (byte row=0; row < rowcountfont/divisor*size; row++) {
      if (color==5){
        showcolor=random(3)+1;
      }
      else
      { 
        showcolor=color;
      }
      if (secondcolor==5){
        showsecondcolor=random(3)+1;
      }
      else
      { 
        showsecondcolor=secondcolor;
      }

      rr=row/size;
      if (dots & (maximumdrawfont>>rr))           
        plot(x+col, y+row, showcolor);        
      else 
        plot(x+col, y+row, showsecondcolor);
      if (divisor>=2){
        if (dots2 & (maximumdrawfont>>rr))           
          plot(x+col, y+rowcountfont/divisor+row, showcolor);
        else 
          plot(x+col, y+rowcountfont/divisor+row, showsecondcolor);
      }
      if (divisor>=3){
        if (dots3& (maximumdrawfont>>rr))           
          plot(x+col, y+2*rowcountfont/divisor+row, showcolor);
        else 
          plot(x+col, y+2*rowcountfont/divisor+row, showsecondcolor);
      }
    }     
  }
}


void ht1632_putcharsize1D(int x, int y,unsigned char c,  char size, byte color,  byte secondcolor,unsigned char * fontname,  char columncountfont, char rowcountfont, char oddeven)
{

  unsigned short dots, dots2,dots3, cc,cc2, cc3, rr, g, t, t3, divisor, arrayposition;

  byte maximumdrawfont, showcolor,showsecondcolor; //128 for the large fonts (=8x8 and 12x8), 64 for all smaller ones
  if  (rowcountfont<=7)
    maximumdrawfont=64;
  else
    maximumdrawfont=128;

  for (byte col=0; col<columncountfont*size ; col++) {
    // Addressing the right 8 lines because 'The Dot Factory' starts from the bottom, all others from top  
    if (rowcountfont <=8) {
      if (c==' ')
        arrayposition=0;
      else
        arrayposition=c*columncountfont;
      /* if  (rowcountfont<=7&&columncountfont <7)
       maximumdrawfont=64;
       else
       maximumdrawfont=128;*/
      cc=col/size+arrayposition;
      dots = pgm_read_byte_near(&fontname[cc]);
      divisor=1;
    } 
    else if (rowcountfont>8 && rowcountfont <=16){
      if (oddeven=='T'){
        g=0;
        t=1;
      }
      else {
        g=1;
        t=0;
      }
      if (c==' ')
        arrayposition=0;
      else
        arrayposition=c*columncountfont*2;
      //maximumdrawfont=48;
      cc=col/size*2+g+arrayposition;
      cc2=col/size*2+t+arrayposition;
      dots = pgm_read_byte_near(&fontname[cc]);
      dots2 = pgm_read_byte_near(&fontname[cc2]);  
      divisor=2;  
    }
    else if (rowcountfont>16 && rowcountfont <=24){
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
      }
      else {
        g=2;
        t=1;
        t3=0;
      }
      divisor=3;
      if (c==' ')
        arrayposition=0;
      else
        arrayposition=c*columncountfont*divisor;
      cc=col/size*divisor+g+arrayposition;
      cc2=col/size*divisor+t+arrayposition;
      cc3=col/size*divisor+t3+arrayposition;
      dots = pgm_read_byte_near(&fontname[cc]);
      dots2 = pgm_read_byte_near(&fontname[cc2]);  
      dots3 = pgm_read_byte_near(&fontname[cc3]); 

    }
    for (byte row=0; row < rowcountfont/divisor*size; row++) {
      if (color==5){
        showcolor=random(3)+1;
      }
      else
      { 
        showcolor=color;
      }
      if (secondcolor==5){
        showsecondcolor=random(3)+1;
      }
      else
      { 
        showsecondcolor=secondcolor;
      }
      rr=row/size;
      if (dots & (maximumdrawfont>>rr))           
        plot(x+col, y+row, showcolor);        
      else 
        plot(x+col, y+row, showsecondcolor);
      if (divisor>=2){
        if (dots2 & (maximumdrawfont>>rr))           
          plot(x+col, y+rowcountfont/divisor+row, showcolor);

        else 
          plot(x+col, y+rowcountfont/divisor+row, showsecondcolor);
      }
      if (divisor>=3){
        if (dots3& (maximumdrawfont>>rr))           
          plot(x+col, y+2*rowcountfont/divisor+row, showcolor);
        else 
          plot(x+col, y+2*rowcountfont/divisor+row, showsecondcolor);
      }
    }     
  }

}


/*
 * Copy a bitmap from any XBM bitmap file 
 * Maximum possible lines are 80 lines
 * Restrictions=Amount of SRAM is the maximum drawing space 
 * There will be a fix for the restrictons hopefully in the future 
 * Slightly adopted for using bitmaps
 *  ht1632_putbigbitmap(x location, y location , bitmapname ,  colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK), number of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory bitmap arrays).
 */

void ht1632_putbigbitmap(int x, int y, byte color, byte secondcolor,  unsigned char * bitmapname, int columncountbitmap, byte rowcountbitmap, char oddeven)
{
  unsigned short dots, dots2,dots3,dots4,dots5, dots6, dots7,dots8,dots9,dots10, cc,cc2,cc3, cc4,cc5,cc6,cc7,cc8, cc9,cc10, g,t,t3,t4,t5,t6,t7,t8,t9,t10, divisor, startcolum, endcolumn;

  byte maximumdrawbitmap=128, showcolor, showsecondcolor; //128 for the large fonts (=8x8 and 12x8), 64 for all smaller ones
  if (x>=0){
    startcolum=0;
  }
  else{
    startcolum=-x;
  }

  if ((-x+X_MAX+2)>=columncountbitmap){
    endcolumn=columncountbitmap;
  }
  else{
    endcolumn=-x+X_MAX+2;
  }



  for (unsigned short col=startcolum; col<endcolumn; col++) {
    //for (byte col=0; col<columncountbitmap; col++) {
    // Addressing the right 8 lines because 'The Dot Factory' starts from the bottom, all others from top  
    if (rowcountbitmap <=8) {
      cc=col;
      dots = pgm_read_byte_near(&bitmapname[cc]);
      divisor=1;
    } 
    else if (rowcountbitmap>8 && rowcountbitmap <=16){
      if (oddeven=='T'){
        g=0;
        t=1;
      }
      else {
        g=1;
        t=0;
      }

      cc=col*2+g;
      cc2=col*2+t;
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);  
      divisor=2;
    }
    else if (rowcountbitmap>16 && rowcountbitmap <=24){     
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
      }
      else {
        g=2;
        t=1;
        t3=0;
      }
      cc=col*3+g;
      cc2=col*3+t; 
      cc3=col*3+t3;   
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);
      dots3 = pgm_read_byte_near(&bitmapname[cc3]);
      divisor=3;
    }
    else if (rowcountbitmap>24 && rowcountbitmap <=32){
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
        t4=3;
      }
      else {
        g=3;
        t=2;
        t3=1;
        t4=0;
      }
      cc=col*4+g;
      cc2=col*4+t; 
      cc3=col*4+t3;
      cc4=col*4+t4; 
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);
      dots3 = pgm_read_byte_near(&bitmapname[cc3]);
      dots4 = pgm_read_byte_near(&bitmapname[cc4]);
      divisor=4;
    }
    else if (rowcountbitmap>32 && rowcountbitmap <=40){   
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
        t4=3;
        t5=4;
      }
      else {
        g=4;
        t=3;
        t3=2;
        t4=1;
        t5=0;
      }
      cc=col*5+g;
      cc2=col*5+t; 
      cc3=col*5+t3;
      cc4=col*5+t4; 
      cc5=col*5+t5; 
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);
      dots3 = pgm_read_byte_near(&bitmapname[cc3]);
      dots4 = pgm_read_byte_near(&bitmapname[cc4]);
      dots5 = pgm_read_byte_near(&bitmapname[cc5]);
      divisor=5;
    }
    else if (rowcountbitmap>40 && rowcountbitmap <=48){   
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
        t4=3;
        t5=4;
        t6=5;
      }
      else {
        g=5;
        t=4;
        t3=3;
        t4=2;
        t5=1;
        t6=0;
      }
      divisor=6;
      cc=col*6+g;
      cc2=col*divisor+t; 
      cc3=col*divisor+t3;
      cc4=col*divisor+t4; 
      cc5=col*divisor+t5; 
      cc6=col*divisor+t6;
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);
      dots3 = pgm_read_byte_near(&bitmapname[cc3]);
      dots4 = pgm_read_byte_near(&bitmapname[cc4]);
      dots5 = pgm_read_byte_near(&bitmapname[cc5]);
      dots6 = pgm_read_byte_near(&bitmapname[cc6]);
    }
    else if (rowcountbitmap>48 && rowcountbitmap <=56){   
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
        t4=3;
        t5=4;
        t6=5;
        t7=6;
      }
      else {
        g=6;
        t=5;
        t3=4;
        t4=3;
        t5=2;
        t6=1;
        t7=0;
      }
      divisor=7;
      cc=col*divisor+g;
      cc2=col*divisor+t; 
      cc3=col*divisor+t3;
      cc4=col*divisor+t4; 
      cc5=col*divisor+t5; 
      cc6=col*divisor+t6;
      cc7=col*divisor+t7;
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);
      dots3 = pgm_read_byte_near(&bitmapname[cc3]);
      dots4 = pgm_read_byte_near(&bitmapname[cc4]);
      dots5 = pgm_read_byte_near(&bitmapname[cc5]);
      dots6 = pgm_read_byte_near(&bitmapname[cc6]);
      dots7 = pgm_read_byte_near(&bitmapname[cc7]);
    }
    else if (rowcountbitmap>56 && rowcountbitmap <=64){   
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
        t4=3;
        t5=4;
        t6=5;
        t7=6;
        t8=7;
      }
      else {
        g=7;
        t=6;
        t3=5;
        t4=4;
        t5=3;
        t6=2;
        t7=1;
        t8=0;
      }
      divisor=8;
      cc=col*divisor+g;
      cc2=col*divisor+t; 
      cc3=col*divisor+t3;
      cc4=col*divisor+t4; 
      cc5=col*divisor+t5; 
      cc6=col*divisor+t6;
      cc7=col*divisor+t7;
      cc8=col*divisor+t8;
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);
      dots3 = pgm_read_byte_near(&bitmapname[cc3]);
      dots4 = pgm_read_byte_near(&bitmapname[cc4]);
      dots5 = pgm_read_byte_near(&bitmapname[cc5]);
      dots6 = pgm_read_byte_near(&bitmapname[cc6]);
      dots7 = pgm_read_byte_near(&bitmapname[cc7]);
      dots8 = pgm_read_byte_near(&bitmapname[cc8]);
    }
    else if (rowcountbitmap>64 && rowcountbitmap <=72){   
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
        t4=3;
        t5=4;
        t6=5;
        t7=6;
        t8=7;
        t9=8;
      }
      else {
        g=8;
        t=7;
        t3=6;
        t4=5;
        t5=4;
        t6=3;
        t7=2;
        t8=1;
        t9=0;
      }
      divisor=9;
      cc=col*divisor+g;
      cc2=col*divisor+t; 
      cc3=col*divisor+t3;
      cc4=col*divisor+t4; 
      cc5=col*divisor+t5; 
      cc6=col*divisor+t6;
      cc7=col*divisor+t7;
      cc8=col*divisor+t8;
      cc9=col*divisor+t9;
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);
      dots3 = pgm_read_byte_near(&bitmapname[cc3]);
      dots4 = pgm_read_byte_near(&bitmapname[cc4]);
      dots5 = pgm_read_byte_near(&bitmapname[cc5]);
      dots6 = pgm_read_byte_near(&bitmapname[cc6]);
      dots7 = pgm_read_byte_near(&bitmapname[cc7]);
      dots8 = pgm_read_byte_near(&bitmapname[cc8]);
      dots9 = pgm_read_byte_near(&bitmapname[cc9]);
    }
    else if (rowcountbitmap>72 && rowcountbitmap <=80){   
      if (oddeven=='T'){
        g=0;
        t=1;
        t3=2;
        t4=3;
        t5=4;
        t6=5;
        t7=6;
        t8=7;
        t9=8;
        t10=9;
      }
      else {
        g=9;
        t=8;
        t3=7;
        t4=6;
        t5=5;
        t6=4;
        t7=3;
        t8=2;
        t9=1;
        t10=0;
      }
      divisor=10;
      cc=col*divisor+g;
      cc2=col*divisor+t; 
      cc3=col*divisor+t3;
      cc4=col*divisor+t4; 
      cc5=col*divisor+t5; 
      cc6=col*divisor+t6;
      cc7=col*divisor+t7;
      cc8=col*divisor+t8;
      cc9=col*divisor+t9;
      cc10=col*divisor+t10;
      dots = pgm_read_byte_near(&bitmapname[cc]);
      dots2 = pgm_read_byte_near(&bitmapname[cc2]);
      dots3 = pgm_read_byte_near(&bitmapname[cc3]);
      dots4 = pgm_read_byte_near(&bitmapname[cc4]);
      dots5 = pgm_read_byte_near(&bitmapname[cc5]);
      dots6 = pgm_read_byte_near(&bitmapname[cc6]);
      dots7 = pgm_read_byte_near(&bitmapname[cc7]);
      dots8 = pgm_read_byte_near(&bitmapname[cc8]);
      dots9 = pgm_read_byte_near(&bitmapname[cc9]);
      dots10 = pgm_read_byte_near(&bitmapname[cc10]);
    }

    for (byte row=0; row < rowcountbitmap/divisor; row++) {
      if (color==5){
        showcolor=random(3)+1;
      }
      else
      { 
        showcolor=color;
      }
      if (secondcolor==5){
        showsecondcolor=random(3)+1;
      }
      else
      { 
        showsecondcolor=secondcolor;
      }

      if (dots & (maximumdrawbitmap>>row))           
        plot(x+col, y+row,  showcolor);
      else 
        plot(x+col, y+row, showsecondcolor);
      if (divisor>=2){ 
        if (dots2 & (maximumdrawbitmap>>row))           
          plot(x+col, y+rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+rowcountbitmap/divisor+row, showsecondcolor);
      }
      if (divisor>=3){
        if (dots3 & (maximumdrawbitmap>>row))           
          plot(x+col, y+2*rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+2*rowcountbitmap/divisor+row, showsecondcolor);
      }
      if (divisor>=4){   
        if (dots4 & (maximumdrawbitmap>>row))           
          plot(x+col, y+3*rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+3*rowcountbitmap/divisor+row, showsecondcolor);
      }
      if (divisor>=5){   
        if (dots5 & (maximumdrawbitmap>>row))           
          plot(x+col, y+4*rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+4*rowcountbitmap/divisor+row, showsecondcolor);
      }
      if (divisor>=6){   
        if (dots6& (maximumdrawbitmap>>row))           
          plot(x+col, y+5*rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+5*rowcountbitmap/divisor+row, showsecondcolor);
      }
      if (divisor>=7){   
        if (dots7& (maximumdrawbitmap>>row))           
          plot(x+col, y+6*rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+6*rowcountbitmap/divisor+row, showsecondcolor);
      }
      if (divisor>=8){   
        if (dots8& (maximumdrawbitmap>>row))           
          plot(x+col, y+7*rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+7*rowcountbitmap/divisor+row, showsecondcolor);
      }
      if (divisor>=9){   
        if (dots9& (maximumdrawbitmap>>row))           
          plot(x+col, y+8*rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+8*rowcountbitmap/divisor+row, showsecondcolor);
      }
      if (divisor>=10){   
        if (dots10& (maximumdrawbitmap>>row))           
          plot(x+col, y+9*rowcountbitmap/divisor+row, showcolor);
        else 
          plot(x+col, y+9*rowcountbitmap/divisor+row, showsecondcolor);
      }

    }    
  }
}



/***********************************************************************
 * Scrolling  functions 
 * for scrolling text and bitmaps
 * Please take only fonts with fixed heigth and width, otherwise the
 * chars will be overlapping
 * Original functions by Bill Ho
 ***********************************************************************/

/*
 * scrollbitmapx()
 * Scrolls a bitmap from left to right
 * Original function by Bill Ho
 * scrollbitmapx (x location, colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK), bitmapnae , number of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory bitmap arrays, delaytime in milliseconds).
 */




void scrollbitmapxcolor(int y, byte color, byte secondcolor,unsigned char * bitmapname,int columncountbitmap, byte rowcountbitmap,char oddeven,int delaytime) {
  byte showcolor,showsecondcolor;
  int xa = 0;
  while (xa<1) {
    int xpos = X_MAX;
    while (xpos > (-1 * ( columncountbitmap))) {
      for (int i = 0; i < 1; i++) {
         if (color==4){
          showcolor=random(3)+1;
        }
        else
        { 
          showcolor=color;}
          if (secondcolor==4){
        showsecondcolor=random(3)+1;
      }
      else
      { 
        showsecondcolor=secondcolor;
      }
          
        ht1632_putbigbitmap(xpos + (columncountbitmap *i), y, showcolor, showsecondcolor, bitmapname , columncountbitmap,rowcountbitmap, oddeven); 
      }
      delay(delaytime);// reduce speed of scroll
      xpos--;
    }
    xa =1;
  }
}

/*
 * scrollbitmapy()
 * Scrolls a bitmap from bottom to up
 * Original function by Bill Ho
 * scrollbitmapy (y location, colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK), bitmapnae , number of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory bitmap arrays, delaytime in milliseconds).
 */
void scrollbitmapycolor(int x,byte color, byte secondcolor,unsigned char * bitmapname ,int columncountbitmap, byte rowcountbitmap,char oddeven, int delaytime) {  
  byte showcolor,showsecondcolor;
  int ya = 0;
  while (ya<1) {
    int ypos = Y_MAX;
    while (ypos > (-1 * ( rowcountbitmap))) {
      for (int i = 0; i < 1; i++) {
         if (color==4){
          showcolor=random(3)+1;
        }
        else
        { 
          showcolor=color;}
          if (secondcolor==4){
        showsecondcolor=random(3)+1;
      }
      else
      { 
        showsecondcolor=secondcolor;
      }
        ht1632_putbigbitmap(x, ypos + ( rowcountbitmap *i), showcolor, showsecondcolor,  bitmapname ,columncountbitmap,rowcountbitmap, oddeven); 
      }
      delay(delaytime);// reduce speed of scroll
      ypos --;    
    }
    ya = 1;
  }
}

/*
 * scrolltextcolor()
 * Scrolls a text string from left to right
 * Simple function for the original ht1632_putchar method without MULTICOLOR and no background color
 * Original function by Bill Ho
 * scrolltextxcolor(y location, string ,  colorname (RANDOMCOLOR for random color), delaytime in milliseconds) 
 */


void scrolltextxcolor(int y,char Str1[ ], byte color, int delaytime){  
  int messageLength = strlen(Str1)+ 1;
  byte showcolor;
  int xa = 0;
  while (xa<1) {
    int xpos = X_MAX;
    while (xpos > (-1 * ( 6*messageLength))) {
      for (int i = 0; i < messageLength; i++) {

        if (color==4){
          showcolor=random(3)+1;
        }
        else
        { 
          showcolor=color;
        }
        ht1632_putchar(xpos + (6* i),  y,Str1[i],showcolor);


      }
      delay(delaytime);// reduce speed of scroll
      xpos--;
    }
    xa =1;
  }
}

/*
 * scrolltextxsize()
 * Scrolls a text string from left to right
 * Original function by Bill Ho
 *  scrolltextxsizexcolor(y location, string ,  size as integer,  colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK), name of the font array,  umber of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory font arrays, delaytime in milliseconds) 
 */
void scrolltextsizexcolor(int y,char Str1[ ], char size, byte color, byte secondcolor,  unsigned char fontname[][NCOLUMNS], int columncountfont, char rowcountfont, char oddeven, int delaytime){
   int messageLength = strlen(Str1)+ 1;
    byte showcolor,showsecondcolor;
  int xa = 0;
  while (xa<1) {
    int xpos = X_MAX;
    while (xpos > (-1 * ( columncountfont*size* messageLength))) {
      for (int i = 0; i < messageLength; i++) {
        if (color==4){
          showcolor=random(3)+1;
        }
        else
        { 
          showcolor=color;}
          if (secondcolor==4){
        showsecondcolor=random(3)+1;
      }
      else
      { 
        showsecondcolor=secondcolor;
      }
        ht1632_putcharsizecolor(xpos + (columncountfont*size * i),  y,Str1[i],   size,   showcolor, showsecondcolor,  fontname,  columncountfont, rowcountfont,  oddeven);
        
      }
      delay(delaytime);// reduce speed of scroll
      xpos--;
    }
    xa =1;
  }
}


/*
 * scrolltextysize()
 * Scrolls a text string from bottom to up
 * Original function by Bill Ho
 *  scrolltextxsizeycolor(y location, string ,  size as integer, colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK), name of the font array,  number of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory font arrays, delaytime in milliseconds) 
 */
 void scrolltextsizey(int x,char Str1[ ], char size, byte color, byte secondcolor, unsigned char fontname[][NCOLUMNS], int columncountfont, char rowcountfont, char oddeven, int delaytime){
   int messageLength = strlen(Str1)+ 1;
    byte showcolor,showsecondcolor;
  int ya = 0;
  while (ya<1) {
    int ypos = Y_MAX;
    while (ypos > (-1 * ( columncountfont*size* messageLength))) {
      for (int i = 0; i < messageLength; i++) {
        if (color==4){
          showcolor=random(3)+1;
        }
        else
        { 
          showcolor=color;}
          if (secondcolor==4){
        showsecondcolor=random(3)+1;
      }
      else
      { 
        showsecondcolor=secondcolor;
      }
        ht1632_putcharsizecolor(x,ypos + (columncountfont*size * i),Str1[i],   size,  showcolor, showsecondcolor, fontname,  columncountfont, rowcountfont,  oddeven);
        
      }
      delay(delaytime);// reduce speed of scroll
      ypos--;
    }
    ya =1;
  }
}

/*
 * scrolltextsize1Dxcolor()
 * Scrolls a text string from left to right
 * Original function by Bill Ho
 *  scrolltextsize1Dxcolor(x location, string ,  size as integer, colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK), name of the font array,  number of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory font arrays, delaytime in milliseconds) 
 */
void scrolltextsize1Dxcolor(int y, char Str1[ ], char size,  byte color, byte secondcolor, unsigned char * fontname,   int columncountfont, char rowcountfont, char oddeven, int delaytime){
   int messageLength = strlen(Str1)+1;
    byte showcolor, showsecondcolor;
  int xa = 0;
  while (xa<1) {
    int xpos = X_MAX;
    while (xpos > (-1 * ( columncountfont*size* messageLength))) {
      for (int i = 0; i < messageLength; i++) {
        if (color==4){
          showcolor=random(3)+1;
        }
        else
        { 
          showcolor=color;}
          if (secondcolor==4){
        showsecondcolor=random(3)+1;
      }
      else
      { 
        showsecondcolor=secondcolor;
      }
        ht1632_putcharsize1D(xpos + (columncountfont*size * i),  y,Str1[i],   size,  showcolor, showsecondcolor,   fontname,  columncountfont, rowcountfont,  oddeven);
      
      }
      delay(delaytime);// reduce speed of scroll
      xpos--;
    }
    xa =1;
  }
}


/***********************************************************************
 * Demo Bitmap data 
 * replace it with your own data
 *
***********************************************************************
 * How to make bitmaps for the matrix
 * 1. Gimp
*  Either import or draw your bitmap.
*  For using the bitmaps with the matrix, you have to rotate it clockwise 90°.
*  Save it as X-Bitmap, don't check the X10 option.
*  Then open the xbm file, add PROGMEM before or after the bitmap name(=bitmap name+"_bits[]"), call the bitmap by with its name.
*
*  2. The Dot Factory (http://www.pavius.net/downloads/tools/53-the-dot-factory)
*  This a good windows software for using fonts but bitmaps, too.
*  For using bitmaps/fonts select the tool icon, check "Flip X", "90°" at Flip/Rotate. "Width" at Fixed, "MsbFirst" if it wasn't.
*  You can import any bitmap.
*  Ppush Generate, replace the "const uint_8" " with "unsigned char PROGMEM" and call the bitmap by with its name.

 ***********************************************************************/

static unsigned char PROGMEM lady_bits[] = {  
0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, //                                              #                          
	0x00, 0x01, 0xFF, 0xFF, 0x00, 0x3C, 0x00, 0x00, 0x00, //                #################          ####                          
	0x00, 0x07, 0xFF, 0xFF, 0x80, 0xFC, 0x00, 0x00, 0x00, //              ####################       ######                          
	0x00, 0x07, 0xFF, 0xFF, 0x83, 0xFC, 0x00, 0x00, 0x00, //              ####################     ########                          
	0x00, 0x0F, 0xFF, 0xFF, 0x1F, 0xFC, 0x00, 0x00, 0x00, //             ####################   ###########                          
	0x00, 0x0F, 0xC0, 0x00, 0x7F, 0xFC, 0x00, 0x00, 0x00, //             ######               #############                          
	0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, //             #######################################################     
	0x1F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, //    #####    ########################################################    
	0x7F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, //  #########  ########################################################    
	0x7F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, //  #########  ########################################################    
	0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, // ########### #######################################################     
	0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, // ########### ##################################                          
	0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, // ########### #######################################################     
	0x7F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, //  #########  ########################################################    
	0x7F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, //  #########  ########################################################    
	0x1F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, //    #####    ########################################################    
	0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, //             #######################################################     
	0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, //             ##################################                          
	0x00, 0x0F, 0xC0, 0x00, 0x7F, 0xFC, 0x00, 0x00, 0x00, //             ######               #############                          
	0x00, 0x0F, 0xFF, 0xFF, 0x07, 0xFC, 0x00, 0x00, 0x00, //             ####################     #########                          
	0x00, 0x07, 0xFF, 0xFF, 0x80, 0x7C, 0x00, 0x00, 0x00, //              ####################        #####                          
	0x00, 0x07, 0xFF, 0xFF, 0x80, 0x0C, 0x00, 0x00, 0x00, //              ####################           ##                          
	0x00, 0x01, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, //                #################                                       
};





static unsigned char a0_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char PROGMEM a1_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00 };

static unsigned char PROGMEM a2_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x08, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char PROGMEM a3_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00,
  0x00, 0x40, 0x01, 0x00, 0x00, 0x48, 0x01, 0x00, 0x00, 0x80, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char PROGMEM a4_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x00, 0x00, 0x00, 0x24, 0x22, 0x00, 0x00, 0x20, 0x27, 0x00,
  0x00, 0x90, 0x03, 0x00, 0x00, 0xa4, 0x02, 0x00, 0x00, 0xc4, 0x11, 0x00,
  0x00, 0x40, 0x21, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x00, 0x00, 0x08, 0x00, 0x00, 0x30, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


static unsigned char PROGMEM a5_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x66, 0x41, 0x00,
  0x00, 0x41, 0x2b, 0x00, 0x00, 0x60, 0x21, 0x00, 0x00, 0x61, 0x16, 0x00,
  0x80, 0x9a, 0x74, 0x00, 0x80, 0x09, 0xc0, 0x01, 0x00, 0x0d, 0xc2, 0x01,
  0x00, 0x11, 0x69, 0x00, 0x00, 0x0d, 0xdd, 0x00, 0x00, 0x0a, 0x42, 0x00,
  0x00, 0xd0, 0x20, 0x00, 0x00, 0xd0, 0x41, 0x00, 0x00, 0x14, 0x20, 0x00,
  0x00, 0x08, 0x04, 0x00, 0x00, 0x40, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char PROGMEM a6_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0f, 0x00,
  0x00, 0xa0, 0x11, 0x00, 0x00, 0xaa, 0x64, 0x00, 0x00, 0x00, 0x29, 0x00,
  0x00, 0x38, 0x38, 0x00, 0x00, 0x42, 0xa9, 0x00, 0x00, 0x26, 0x72, 0x01,
  0x00, 0x11, 0x4c, 0x00, 0x00, 0x25, 0x90, 0x02, 0x00, 0x11, 0x40, 0x00,
  0x40, 0x51, 0xe8, 0x00, 0x00, 0x1b, 0xe1, 0x03, 0x80, 0xb1, 0x62, 0x00,
  0x00, 0x68, 0x0e, 0x01, 0x00, 0xf3, 0x10, 0x00, 0x00, 0xd0, 0x9f, 0x00,
  0x00, 0x24, 0x5c, 0x00, 0x00, 0x80, 0x22, 0x00, 0x00, 0x20, 0x08, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


static unsigned char PROGMEM a7_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0xa0, 0x1c, 0x00,
  0x00, 0x6c, 0x31, 0x00, 0x00, 0x30, 0x15, 0x00, 0x00, 0x06, 0x61, 0x00,
  0x00, 0x24, 0x8a, 0x00, 0x80, 0x75, 0xd9, 0x00, 0x80, 0x34, 0x88, 0x01,
  0x80, 0x1c, 0x04, 0x01, 0x40, 0x45, 0x20, 0x00, 0x00, 0x00, 0x60, 0x00,
  0xc0, 0x85, 0xc0, 0x02, 0xc0, 0x54, 0xd0, 0x02, 0x00, 0x0a, 0x00, 0x01,
  0x80, 0x04, 0xc8, 0x00, 0x00, 0x15, 0x10, 0x01, 0x00, 0x80, 0x15, 0x00,
  0x00, 0xc5, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0xa4, 0x3c, 0x00,
  0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static unsigned char PROGMEM preisinfomation3_bits[] = {
  0x00,0x00, 0x1e, 0x00, 0x2d, 0x00, 0x33, 0x00, 0x33, 0x00, 0x2d, 0x00, 0x1e, 0x00,
  0xf8, 0x27, 0xf8, 0x2b, 0x88, 0x2d, 0xf0, 0x2e, 0x60, 0x2f, 0x00, 0x2f,
  0x00, 0x2f, 0x00, 0x0f, 0x9e, 0x07, 0x6d, 0x00, 0x33, 0x18, 0x33, 0x3c,
  0x2d, 0x18, 0x1e,  0x00 };   

static unsigned char PROGMEM Jahr0_bits[] = {
  0x1F, 0x80, //    ######       
  0x7F, 0xE0, //  ##########     
  0xC0, 0x30, // ##        ##    
  0x80, 0x10, // #          #    
  0x80, 0x10, // #          #    
  0xC0, 0x30, // ##        ##    
  0x7F, 0xE0, //  ##########     
  0x1F, 0x80};    
static unsigned char PROGMEM Jahr1_bits[] = {
  // @16 '1' (8 pixels wide)
  0x00, 0x00, //                 
  0x40, 0x10, //  #         #    
  0x40, 0x10, //  #         #    
  0xFF, 0xF0, // ############    
  0xFF, 0xF0, // ############    
  0x00, 0x10, //            #    
  0x00, 0x10, //            #    
  0x00, 0x00};    
static unsigned char PROGMEM Jahr2_bits[] = {                

  // @32 '2' (8 pixels wide)
  0x20, 0x10, //   #        #    
  0x40, 0x30, //  #        ##    
  0x80, 0x70, // #        ###    
  0x81, 0xB0, // #      ## ##    
  0xC3, 0x30, // ##    ##  ##    
  0xFC, 0x30, // ######    ##    
  0x78, 0x30, //  ####     ##    
  0x00, 0x60 //          ##   
};  




/***********************************************************************
 * Utility function to compute the fps of the display. Call it each time you
 * consider you displayed a new frame.
 ***********************************************************************/
void compute_fps() {
#ifdef COMPUTE_FPS
  static const byte kComputeEveryFrames = 250;
  static unsigned long last_millis = 0;
  static byte cnt_frames = 0;
  if (++cnt_frames == kComputeEveryFrames) {
    cnt_frames = 0;
    int fps = kComputeEveryFrames / ((millis() - last_millis) / 1000);
    char fps_str[15];
    sprintf(fps_str, "%d fps", fps);
    Serial.println(fps_str); 
    last_millis = millis();
  }
#endif  // COMPUTE_FPS
}


// Vertical lines moving from right to left. With this pattern, at each new
// frame half of the pixels are completely modified (i.e. both in the red and green
// plane), and half change only in one plane (i.e. only red or green).
// This leads to ~15fps with two screens.
void launch_fps_benchmark() {
  for (byte k = 0; ; k = (k + 1) % (ORANGE + 1)) {
    for (int i = 0; i <= Y_MAX; i++) {
      for (int j = 0; j <= X_MAX; ++j) {
	plot(j, i, k++);
	if (k > ORANGE) k = 0;
      }
    }
    compute_fps();
  }
}

/***********************************************************************
 * traditional Arduino sketch functions: setup and loop.
 ***********************************************************************/

void setup ()  // flow chart from page 17 of datasheet
{
  ht1632_setup(); 
  //randomSeed(analogRead(0));
  
  Serial.begin(9600);

  cls();
}

void loop ()
{
  launch_fps_benchmark(); return;  // Comment this out to test other demos!

  
//ht1632_putbigbitmap(0,0 , RED, GREEN, lady_bits,  23,72, 'T'); 
//delay(LONGDELAY);


// Scrolls the Bitmap from left to right  
// scrollbitmapx (x location,colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK),bitmapnae , number of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory bitmap arrays, delaytime in milliseconds).
//scrollbitmapxcolor (0,RANDOMCOLOR, ORANGE,lady_bits, 23,72,'T', 30);

//delay(LONGDELAY);


//Scrolls a bitmap from bottom to up
// scrollbitmapx (x location,colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK),bitmapnae , number of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory bitmap arrays, delaytime in milliseconds).
//scrollbitmapycolor (0,MULTICOLOR, 0,lady_bits , 23,72,'T', 30);

//delay(LONGDELAY);

// Scrolls the String from left to right  
// scrolltextxsize(y location, string ,  size as integer, colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK), name of the font array,  number of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory font arrays, delaytime in milliseconds) 

//scrolltextsizexcolor(1,"Donaudampfschifffahrtskapitaenskajuete    ",1,RANDOMCOLOR, 0,my2font,8,8,'G',0); //longest German word, don't forget the space chars on the end

//scrolltextsizexcolor(1,"John 3:16 - For God so loved the world that he gave his only begotten son that whosoever believe in him shall be saved.    ",1,GREEN, 0,my2font,8,8,'G',0); //don't forget the space chars on the end

//delay(LONGDELAY);



//scrolltextsizexcolor(1,"Arduino rocks    ",2,RANDOMCOLOR, 0,my2font,8,8,'G',0); // don't forget the space chars on the end
//delay(LONGDELAY);

// Scrolls the String from down to up  
// scrolltextxsizey(x location, string ,  size as integer, colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK), name of the font array,  umber of columns, number of rows, 'G' for Gimp or 'T' for The Dot Facto

//scrolltextsizey(0,"Test    ",1,GREEN, 0,my2font,8,8,'G',0); //don't forget the space chars on the end
//delay(LONGDELAY);

//Draws a char
// ht1632_putcharsizecolor(x location, y location , char ,  size as integer,  colorname (RANDOMCOLOR for random color), name of the second color (instead of BLACK),name of the font array,  umber of columns, number of rows, 'G' for Gimp or 'T' for The Dot Factory font arrays) 
//ht1632_putcharsizecolor(0,0,'B',2,MULTICOLOR,BLACK,my2font,8,8,'G');

//delay(LONGDELAY);
//cls();

//  ht1632_putbigbitmap(0, -8, RED, 0, a0_bits , 32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap(0, -8, RED, 0, a1_bits , 32,32, 'G');  
//  delay(100);
//  ht1632_putbigbitmap(0, -8, RED,  0,a2_bits , 32,32, 'G'); 
//  delay(100);
//  ht1632_putbigbitmap(-7, -6,GREEN,  0, a0_bits , 32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap(0, -8,  RED,  0,a3_bits , 32,32, 'G'); 
//  delay(100);
//  ht1632_putbigbitmap(-7, -6, GREEN,  0, a1_bits ,32,32, 'G');  
//  delay(100);
//  ht1632_putbigbitmap(0, -8,RED,  0, a4_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap(  -7, -6, GREEN,  0,a2_bits , 32,32, 'G');  
//  delay(100);
//  ht1632_putbigbitmap(0, -8,RED, BLACK, a5_bits , 0,32, 'G'); 
//  delay(100);
//  ht1632_putbigbitmap(  -7, -8,GREEN, 0, a3_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap( 9, -10,ORANGE,BLACK, a0_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap(0, -8, RED,BLACK,a6_bits ,  32,32, 'G');   
//  delay(100); 
//  ht1632_putbigbitmap(  -7, -6,GREEN,BLACK, a4_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap( 9, -10,ORANGE,BLACK, a1_bits , 32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap(  -7, -6, GREEN,BLACK,  a5_bits ,32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap( 9, -10, ORANGE,BLACK, a2_bits , 32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap(  -7, -6, GREEN,BLACK,a6_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap( 9, -10,ORANGE,BLACK,a3_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap(  -7, -6,  GREEN,BLACK,a7_bits , 32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap( 9, -10, ORANGE,BLACK,a4_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap( 9, -10,ORANGE,BLACK, a5_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap( 9, -10, ORANGE,BLACK, a6_bits , 32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap( 9, -10, ORANGE,BLACK,a7_bits ,  32,32, 'G');
//  delay(100);
//  ht1632_putbigbitmap(0, -8,BLACK,BLACK, a0_bits , 32,32, 'G');
  cls();
  delay(200);
//  scrolltextxcolor(4,"HAPPy New year    ",RANDOMCOLOR,30);  
  scrolltextxcolor(4,
		   "For God so loved the world, that he gave his only begotten "
		   "Son, that whosoever believeth in him should not perish, but "
		   "have everlasting life.   ",
		   RED,
		   30); 
  delay(100);
  cls();  
  for (int i=0; i <= 5; i++){
    //ht1632_putbigbitmap(0, 2, RED,ORANGE,Jahr2_bits ,   8,14, 'T');
    //ht1632_putbigbitmap(9, 2, 0,random(3)+1,Jahr0_bits ,   8,14, 'T');
    //ht1632_putbigbitmap(17, 2, MULTICOLOR, 0,Jahr1_bits ,   8,14, 'T');
    //ht1632_putbigbitmap(25, 2, random(3)+1,0,Jahr1_bits ,   8,14, 'T');
    
    ht1632_putbigbitmap(0, 2, RED,ORANGE,Jahr2_bits ,   8,14, 'T');
    ht1632_putbigbitmap(9, 2, 0,GREEN,Jahr0_bits ,   8,14, 'T');
    ht1632_putbigbitmap(17, 2, GREEN, 0,Jahr1_bits ,   8,14, 'T');
    ht1632_putbigbitmap(25, 2, RED,0,Jahr1_bits ,   8,14, 'T');
    
    delay(100);
    }
  delay(5000);
  cls(); 
    
  ht1632_putchar(0, 0, 'J', GREEN);
  ht1632_putchar(6, 0, 'o', GREEN);
  ht1632_putchar(12, 0, 'h', GREEN);
  ht1632_putchar(18, 0, 'n', GREEN);
  ht1632_putchar(24, 0, ' ', BLACK);
  
  ht1632_putchar(30, 0, '3', RED);
  ht1632_putchar(36, 0, ':', ORANGE);
  ht1632_putchar(42, 0, '1', RED);
  ht1632_putchar(48, 0, '6', RED);
  ht1632_putchar(54, 0, '-', ORANGE);
  ht1632_putchar(60, 0, ' ', BLACK);

  ht1632_putchar(0, 8, 'F', GREEN);
  ht1632_putchar(6, 8, 'o', GREEN);
  ht1632_putchar(12, 8, 'r', GREEN);
  ht1632_putchar(18, 8, ' ', BLACK);
  ht1632_putchar(24, 8, 'G', ORANGE);
  
  ht1632_putchar(30, 8, 'o', ORANGE);
  ht1632_putchar(36, 8, 'd', ORANGE);
  ht1632_putchar(42, 8, ' ', BLACK);
  ht1632_putchar(48, 8, 's', GREEN);
  ht1632_putchar(54, 8, 'o', GREEN);
  ht1632_putchar(60, 8, ' ', BLACK);

  delay(5000);

  ht1632_putchar(0, 0, 'l', GREEN);
  ht1632_putchar(6, 0, 'o', GREEN);
  ht1632_putchar(12, 0, 'v', GREEN);
  ht1632_putchar(18, 0, 'e', GREEN);
  ht1632_putchar(24, 0, 'd', GREEN);
  
  ht1632_putchar(30, 0, ' ', BLACK);
  ht1632_putchar(36, 0, 't', GREEN);
  ht1632_putchar(42, 0, 'h', GREEN);
  ht1632_putchar(48, 0, 'e', GREEN);
  ht1632_putchar(54, 0, ' ', BLACK);
  ht1632_putchar(60, 0, ' ', BLACK);

  ht1632_putchar(0, 8, 'w', GREEN);
  ht1632_putchar(6, 8, 'o', GREEN);
  ht1632_putchar(12, 8, 'r', GREEN);
  ht1632_putchar(18, 8, 'l', GREEN);
  ht1632_putchar(24, 8, 'd', GREEN);
  
  ht1632_putchar(30, 8, ' ', BLACK);
  ht1632_putchar(36, 8, 't', GREEN);
  ht1632_putchar(42, 8, 'h', GREEN);
  ht1632_putchar(48, 8, 'a', GREEN);
  ht1632_putchar(54, 8, 't', GREEN);
  ht1632_putchar(60, 8, ' ', BLACK);

  delay(5000);
  cls(); 
}
