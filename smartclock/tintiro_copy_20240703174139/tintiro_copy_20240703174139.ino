void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  attachInterrupt(2, randamCreat, RISING);
  pinMode(5,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:



}

void randamCreat(void){
  int rN1 = random(2);
  int rN2 = random(2);
  int rN3 = random(2);

  //Serial.println(randNumber);
  if(rN1 == rN2 && rN2 == rN3){
    zorome();
  }

}

void zorome(void){
  int melo = 500;
  int pin = 5;
  tone(pin,262,melo); //ド(2)
  delay(melo);
  tone(pin,294,melo); //レ
  delay(melo);
  tone(pin,330,melo); //ミ
  delay(melo);
  tone(pin,349,melo); //ファ
  delay(melo);
  tone(pin,392,melo); //ソ
  delay(melo);
  tone(pin,440,melo); //ラ
  delay(melo);
  tone(pin,494,melo); //シ
  delay(melo);
  tone(pin,523,melo); //ド(3)
  delay(melo);

  delay(1000);

}