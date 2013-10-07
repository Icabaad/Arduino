

const int LED = 9;
const int BUTTON = 7; 
int val = 0; //stores input pin state
int old_val = 0; // stores prev value of val
int state = 0; //0 = off 1 = on
int brightness = 128;
unsigned long startTime = 0; //started pressing?

void setup() {                
pinMode(LED, OUTPUT);
pinMode(BUTTON, INPUT);
}

void loop() {
  
  val = digitalRead(BUTTON); //read input and store it
  
  //check for transition
  if ((val == HIGH) && (old_val == LOW)) {
    
    state = 1 - state; //change state from off to on etc
    
    startTime = millis();
    
    delay(10);
  }
  
  //check if button is held down
  if ((val == HIGH) && (old_val == HIGH)) {
  
  if (state == 1 && (millis() - startTime) > 500) {
    
    brightness++; //inc light +1
    delay(10);
    
    if (brightness > 255) { //255 is max
    
    brightness = 0; // go over 255 go back to 0
    
    }
  }
}

old_val = val; //value now old store it

if (state == 1) {
  analogWrite(LED, brightness); //turn LED on at current brightness
  
} else {
  analogWrite(LED, 0); //led off
}
}

