#include <ht1632c.h>

ht1632c ledMatrix = ht1632c(&PORTD, 5, 3, 4, 2, GEOM_32x16, 2);

//int lols = 1000;

int PIRout = 999;
char test[] = "Piggies";
char PIR[3];
sprintf(PIR, "%i.txt", notes);

void setup() {
  ledMatrix.clear();
  ledMatrix.pwm(15);
  
}

void loop() {
  
  char tmp[10] = "fatties";
  byte len = strlen(test);
  for (int i = 0; i < len; i++)
  ledMatrix.putchar(6*i,  8, PIR[i], ORANGE);
  ledMatrix.sendframe();
  
}
