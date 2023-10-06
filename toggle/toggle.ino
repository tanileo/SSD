void setup() {
  // put your setup code here, to run once:
  pinMode(19, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.begin(9600);
  Serial.println(digitalRead(19));
}
