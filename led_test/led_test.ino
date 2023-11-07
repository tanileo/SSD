int current_led = 0;
void light(int pin){
  int i;
  for(i=0; i<13; i++){
    digitalWrite(i, LOW);
  }
  digitalWrite(pin, HIGH);
}
void setup() {
  int i;
  for(i=0; i<13; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  // pinMode(13, INPUT);
}

void loop() {
  if(digitalRead(13)){
    current_led++;
    if(current_led == 13){
      current_led = 0;
    }
  }
  light(current_led);
  delay(100);
}
