#define PUSH_BUTTON 21

const int pinA[7] = {14, 27, 4, 2, 15, 12, 13};
const int pinB[7] = {33, 32, 17, 5, 16, 25, 26};
bool push_status = 0;
int count_status = 0;
int current_number = 0;

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
  for (i = 0; i < 7; i++) {
    digitalWrite(pinA[i], !bitRead(pattern[num10], i));
  }
  for (i = 0; i < 7; i++) {
    digitalWrite(pinB[i], !bitRead(pattern[num1], i));
  }
}

void push() {
  Serial.print("push");
  push_status = 1;
}

void increment(bool push_status) {
  if (push_status == 1) {
    count_status++;
    push_status = 0;
    if (count_status == 3) {
      count_status = 0;
    } 
  }
  int num = current_number;
  int num10 = num / 10;
  int num1 = num % 10;

  switch (count_status) {
    case 0: num10++; num1++; break;
    case 1: num1++; break;
    case 2: break;
  }
  Serial.print("count_status:");
  Serial.println(count_status);
  if (num10 == 10) {
    num10 = 0;
  }
  if (num1 == 10) {
    num1 = 0;
  }

  current_number = num10 * 10 + num1;
}

void setup() {
  int i;
  Serial.begin(9600);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  attachInterrupt(PUSH_BUTTON, push, FALLING);
  for (i = 0; i < 7; i++) {
    pinMode(pinA[i], OUTPUT);
    pinMode(pinB[i], OUTPUT);
    digitalWrite(pinA[i], HIGH);
    digitalWrite(pinB[i], HIGH);
  }
}

void loop() {
  light(current_number);
  increment(push_status);
  Serial.print("Current Number: ");
  Serial.println(current_number);
  delay(100); // 適度なディレイを挿入
}
