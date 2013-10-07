
#define LEDPin 6
#define onboardLED 13
#define motionPin 4 
#define fadeSpeed 10
#define lightPin 5

int led;
int motion;
int counter = 0;
int light;

void setup() {

  pinMode(LEDPin, OUTPUT);
  pinMode(onboardLED, OUTPUT);
  pinMode(motionPin, INPUT);
  pinMode(lightPin, INPUT);
  Serial.begin(9600);
  int motion = LOW;
  led = 0;
  //delay(5000);
}

void loop() {

  light = analogRead(lightPin);
  Serial.print("Light Level: "); 
  Serial.println(light);  
 // delay(100);

  // digitalRead(motionPin);
  motion = digitalRead(motionPin);
  /* if(motion == 1) {
   digitalWrite(onboardLED, HIGH);
   counter ++;
   }
   else{
   digitalWrite(onboardLED, LOW);
   counter = 0;
   }
   */
  Serial.print("Motion: "); 
  Serial.println(motion);
  Serial.println(led);

  if((light > 500) && (motion == 1)) {
    while (led < 255) {
      led++;
      //for (led = 0; led <= 254; led++);
      analogWrite(LEDPin, led);
      delay(fadeSpeed);
      break;
      //Serial.print("     UP ");
      //Serial.println(led);
      // motion = digitalRead(motionPin);
    }
  }
  else if((motion == 0) && (led > 0)) {
    led--;
    // for (led = 254; led >= 254; led--);
    analogWrite(LEDPin, led);
    delay(fadeSpeed);
    // Serial.print("     DN ");
    // Serial.println(led);
    // motion = digitalRead(motionPin);
  }

  else {
    // delay(250);
    // Serial.println("delay");
  }
}

/*

 for (led = 30; led < 100; led++) { 
 analogWrite(LEDPin, led);
 analogWrite(onboardLED, led);
 delay(fadeSpeed);
 Serial.println( led );
 } 
 
 for (led = 100; led > 30; led--) { 
 analogWrite(LEDPin, led);
 analogWrite(onboardLED, led);
 delay(fadeSpeed);
 Serial.println( led );
 } 
 */




