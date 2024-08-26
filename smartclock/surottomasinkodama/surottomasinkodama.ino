
#define PUSH_BUTTON 21
int push_status=0;
const int pinA[7] = {14, 27, 4, 2, 15, 12, 13};
const int pinB[7] = {33, 32, 17, 5, 16, 25, 26};
const int pinC[7] = {11, 44, 22, 33, 55, 66, 77};

const byte pattern[10] = {
  B00111111, // 0
  B00000110, // 1
  B01011011, // 2
  B01001111, // 3
  B01100110, // 4
  B01101101, // 5
  B01111101, // 6
  B00100111, // 7
  B01111111, // 8
  B01101111  // 9
};

void light(int num) {
  int num100 = num / 100;
  int num10 = num / 10;
  int num1 = num % 10;
  int i;
  for(i = 0; i < 7; i++){
    digitalWrite(pinA[i], !bitRead(pattern[num100], i));
  }
  for(i = 0; i < 7; i++){
    digitalWrite(pinB[i], !bitRead(pattern[num10], i));
  }
  for(i = 0; i < 7; i++){
    digitalWrite(pinC[i], !bitRead(pattern[num1], i));
  }
  
}

void push() {
  // Serial.print("push");
  push_status += 1;
  int num100=0;
  int num10=0;
  int num1=0;
  int num=0;
  if(push_status==0){
    num100++;
    num10++;
    num1++;
  }else if(push_status==1){
    num10++;
    num1++;
  }else{
    num1++;
  }
  num=num100*100+num10*10+num1;
  light(num);
  delay(10);
}

void setup() {
  int i;
  for(i = 0; i<7; i++){
    pinMode(pinA[i], OUTPUT);
    pinMode(pinB[i], OUTPUT);
    pinMode(pinC[i], OUTPUT);
    digitalWrite(pinA[i], HIGH);
    digitalWrite(pinB[i], HIGH);]
    digitalWrite(pinC[i], HIGH);
  }
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  attachInterrupt(PUSH_BUTTON, push, FALLING);

}

void loop() {
  int i = 0;
  // for(i = 0; i<7; i++){
  //   digitalWrite(pinA[i], LOW);
  //   delay(1000);
  //   digitalWrite(pinA[i], HIGH);
  // }
  // for(i = 0; i<7; i++){
  //   digitalWrite(pinB[i], LOW);
  //   delay(1000);
  //   digitalWrite(pinB[i], HIGH);
  // }
  // for(i = 0; i<7; i++){
  //   digitalWrite(pinC[i], LOW);
  //   delay(1000);
  //   digitalWrite(pinC[i], HIGH);
  // }
  for(i = 0; i<100; i++){
    light(i);
    delay(100);
  }

}