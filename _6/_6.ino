const int SENSOR = 0; //input for sensor
int val = 0; //variable  to store value from sensor

void setup() {
  Serial.begin(9600); //open serial at 9600
}

void loop() {
  val = analogRead(SENSOR); //READ VALUE FROM SENSOR
  Serial.println(val); //print value to serial
  delay(10000);
  
}/vnish
