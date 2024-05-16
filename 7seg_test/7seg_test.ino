const int pinA[7] = {14, 27, 4, 2, 15, 12, 13};
const int pinB[7] = {33, 32, 17, 5, 16, 25, 26};

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
  int num10 = num / 10;
  int num1 = num % 10;
  int i;
  for(i = 0; i < 7; i++){
    digitalWrite(pinA[i], !bitRead(pattern[num10], i));
  }
  for(i = 0; i < 7; i++){
    digitalWrite(pinB[i], !bitRead(pattern[num1], i));
  }
  
}

void setup() {
  int i;
  for(i = 0; i<7; i++){
    pinMode(pinA[i], OUTPUT);
    pinMode(pinB[i], OUTPUT);
    digitalWrite(pinA[i], HIGH);
    digitalWrite(pinB[i], HIGH);
  }

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
  for(i = 0; i<100; i++){
    light(i);
    delay(100);
  }

}
