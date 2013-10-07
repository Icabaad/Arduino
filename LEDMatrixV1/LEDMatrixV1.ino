#include <ht1632c.h>

ht1632c ledMatrix = ht1632c(&PORTD, 5, 3, 4, 2, GEOM_32x16, 2);

int number = 1;

void setup() {
  ledMatrix.clear();
  ledMatrix.pwm(15);
  
}

void loop() {
  ledMatrix.setfont(FONT_5x7);
  ledMatrix.putchar(2,5,'S',RED);
  ledMatrix.putchar(6,5,'I');
  ledMatrix.putchar(11,5,'N',ORANGE);
  ledMatrix.putchar(16,5,'G',RED);
  ledMatrix.putchar(21,5,'E');
  ledMatrix.putchar(26,5,'R',ORANGE);
 
  ledMatrix.hscrolltext(8,"MULTICOLOR ",MULTICOLOR, 20, 1, LEFT);
  ledMatrix.hscrolltext(8,"RED ",RED, 10, 1, LEFT);
  ledMatrix.hscrolltext(8,"RED+BLINK ",RED | BLINK, 30, 1, LEFT);
  ledMatrix.hscrolltext(8,"RANDOM COLOR ",RANDOMCOLOR, 50, 1, LEFT);
  ledMatrix.hscrolltext(8,"GREEN ",GREEN, 10, 1, LEFT);
  ledMatrix.hscrolltext(8,"ORANGE ",ORANGE, 10, 1, LEFT);
  ledMatrix.clear();
  
  
  
  ledMatrix.putchar(0, 0, 'number', GREEN, 0);
  ledMatrix.sendframe();
  delay(2000);
  ledMatrix.clear();
  
 // ledMatrix.setfont(FONT_8x16B);
  ledMatrix.hscrolltext(0, "Swordfish", RED, 10, 1, LEFT);
  ledMatrix.clear();
  
  ledMatrix.setfont(FONT_8x16);
  ledMatrix.hscrolltext(0, "Tangerine", RED, 10, 1, RIGHT);
  ledMatrix.clear();
  
  ledMatrix.setfont(FONT_5x7W);
  ledMatrix.vscrolltext(0, "Disco", GREEN, 10, 10, UP);
  ledMatrix.clear();
  
  ledMatrix.setfont(3);
  ledMatrix.vscrolltext(0, "car", ORANGE, 50, 10, DOWN);
  ledMatrix.clear();
  
  ledMatrix.setfont(20);
  ledMatrix.hscrolltext(8, "Random", RED, 50, 1, LEFT);
  ledMatrix.clear();
  
    char tmp[20] = "The end ;-)";
  byte len = strlen(tmp);
  for (int i = 0; i < len; i++)
  ledMatrix.putchar(6*i,  8, tmp[i], ORANGE);
  ledMatrix.sendframe();
  
    byte v;
  for (v = 0; v < 16; v++)
  {
    ledMatrix.pwm(v);
    (v < 4) ? delay(50) : delay (100);
  } 
  for (v = 15; v > 0; v--)
  {
    ledMatrix.pwm(v);
    (v < 4) ? delay(50) : delay (100);
  } 
  for (v = 0; v < 16; v++)
  {
    ledMatrix.pwm(v);
    (v < 4) ? delay(50) : delay (100);
  } 
  for (v = 15; v > 0; v--)
  {
    ledMatrix.pwm(v);
    (v < 4) ? delay(50) : delay (100);
  } 
  delay (1000);
  
  ledMatrix.pwm(7);
  
  for (byte i=0; i<64; i++) {
    for (byte j=0; j<16; j++) {
      ledMatrix.plot(i, j, RED);
      ledMatrix.sendframe();
      delay(5);
    }
  }

  for (byte i=0; i<64; i++) {
    for (byte j=0; j<16; j++) {
      ledMatrix.plot(i, j, GREEN);
      ledMatrix.sendframe();
      delay(5);
    }
  }
  
  for (byte i=0; i<64; i++) {
    for (byte j=0; j<16; j++) {
      ledMatrix.plot(i, j, ORANGE);
      ledMatrix.sendframe();
      delay(5);
    }
  }
  for (byte i=0; i<64; i++) {
    for (byte j=0; j<16; j++) {
      ledMatrix.plot(i, j, BLACK);
      ledMatrix.sendframe();
      delay(5);
    }
  }
}

