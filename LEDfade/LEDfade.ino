
#define LEDPin 6
#define onboardLED 13
#define motionPin 4 
#define fadeSpeed 1000    
 
void setup() {

  pinMode(LEDPin, OUTPUT);
  pinMode(onboardLED, OUTPUT);
  pinMode(motionPin, INPUT);
  Serial.begin(9600);
}
 
 
void loop() {
  int led;
  int motion;
  
  digitalRead(motionPin);
  motion = motionPin;
  Serial.println("Motion: ");
 
  // fade from blue to violet
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
}
