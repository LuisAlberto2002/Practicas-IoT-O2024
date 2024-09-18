
void setup() {
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  

}

void loop() {
  digitalWrite(2,HIGH);
  delay(2500);
  digitalWrite(2,LOW);
  delay(2500);

}
