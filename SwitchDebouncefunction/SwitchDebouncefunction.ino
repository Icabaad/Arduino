const int buttonPin = 2;
const int debounceDelay = 5000;

boolean debounce(int pin) {
  boolean state;
  boolean previousState;
  
  previousState = digitalRead(pin);
  for(int counter=0; counter < debounceDelay; counter++) {
    delay(1);
    state = digitalRead(pin);
    if(state != previousState) {
      counter = 0;
      previousState = state;
    }
  }
  return state;
}
  


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
pinMode(buttonPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly: 
 if (debounce(buttonPin)) {
  Serial.println("Button Pressed for 5 seconds! Run 50 mode");
 }
 else {
   Serial.println("Button not pressed!");
   }
}
