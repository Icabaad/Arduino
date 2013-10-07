

const int LED = 9;
int i =0;

void setup() {                
pinMode(LED, OUTPUT) ;
}

void loop() {
for (i = 0; i < 255; i++) { //loop from 0 -254(fade in )
analogWrite(LED, i); // set led brightness
delay(5); // delay so we can see it :)
}

for (i = 255; i > 0; i--) { //loop from 255 - 0(fade out)

analogWrite(LED, i); //set led bright
delay(5);
}

}

