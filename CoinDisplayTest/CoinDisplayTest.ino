#include <U8glib.h>

U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);
// SPI Com: SCK = en = 3, MOSI = rw = 9, CS = di = 8

int a = 5;
int b = 25;
int c = 15;
int d = 20;
int e = 25;
int g = 30;

int agraph;
int bgraph;
//agraph = map(a,0,100,0,50);

int areverse;
int breverse;


void draw(void) {
// graphic commands to redraw the complete screen should be placed here 
//u8g.setFont(u8g_font_courB12);
u8g.setFont(u8g_font_unifont);
//u8g.setFont(u8g_font_osb21);
//u8g.drawStr( 2, 2, agraph);
u8g.drawFrame(0,0,128,64);
u8g.drawBox(2,areverse,19,(agraph));
u8g.drawBox(22,breverse,19,(bgraph));
u8g.setPrintPos(3,15);
u8g.print(agraph); 
u8g.setPrintPos(12,15);
u8g.print(bgraph); 
}

void setup() {
  // put your setup code here, to run once:
// flip screen, if required
u8g.setRot180();

// set SPI backup if required
//u8g.setHardwareBackup(u8g_backup_avr_spi);
// assign default color value
if ( u8g.getMode() == U8G_MODE_R3G3B2 ) 
u8g.setColorIndex(255); // white
else if ( u8g.getMode() == U8G_MODE_GRAY2BIT )
u8g.setColorIndex(3); // max intensity
else if ( u8g.getMode() == U8G_MODE_BW )
u8g.setColorIndex(1); // pixel on
}

void loop() {
  // put your main code here, to run repeatedly: 
  // picture loop
u8g.firstPage(); 

do {
draw();
} while( u8g.nextPage() );

// rebuild the picture after some delay

agraph = map(a,0,100,0,50);
bgraph = map(b,0,100,0,50);
areverse = 62 - agraph;
breverse = 62 - bgraph;
delay(500);

}
