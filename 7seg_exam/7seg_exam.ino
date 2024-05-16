// ピンの定義
const int digit1A = 14;
const int digit1B = 27;
const int digit1C = 4;
const int digit1D = 2;
const int digit1E = 15;
const int digit1F = 12;
const int digit1G = 13;

const int digit2A = 9;
const int digit2B = 10;
const int digit2C = 11;
const int digit2D = 12;
const int digit2E = 13;
const int digit2F = A0;
const int digit2G = A1;

const int digit1Common = A2; // 第1桁の共通アノード
const int digit2Common = A3; // 第2桁の共通アノード

// 数字のパターン
const byte digitPatterns[] = {
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110  // 9
};

void setup() {
  // デジタルピンを出力に設定
  pinMode(digit1A, OUTPUT);
  pinMode(digit1B, OUTPUT);
  pinMode(digit1C, OUTPUT);
  pinMode(digit1D, OUTPUT);
  pinMode(digit1E, OUTPUT);
  pinMode(digit1F, OUTPUT);
  pinMode(digit1G, OUTPUT);
  
  pinMode(digit2A, OUTPUT);
  pinMode(digit2B, OUTPUT);
  pinMode(digit2C, OUTPUT);
  pinMode(digit2D, OUTPUT);
  pinMode(digit2E, OUTPUT);
  pinMode(digit2F, OUTPUT);
  pinMode(digit2G, OUTPUT);
  
  pinMode(digit1Common, OUTPUT);
  pinMode(digit2Common, OUTPUT);
}

void loop() {
  // 99から0までの数値を表示
  for (int i = 99; i >= 0; i--) {
    int digit1 = i / 10; // 第1桁の数字
    int digit2 = i % 10; // 第2桁の数字
    
    // 第1桁の表示
    displayDigit(digit1, digit1Common, digit1A, digit1B, digit1C, digit1D, digit1E, digit1F, digit1G);
    delay(5); // 数字を表示する時間
    
    // 第2桁の表示
    displayDigit(digit2, digit2Common, digit2A, digit2B, digit2C, digit2D, digit2E, digit2F, digit2G);
    delay(5); // 数字を表示する時間
  }
}

// 数字を表示する関数
void displayDigit(int digit, int commonPin, int pinA, int pinB, int pinC, int pinD, int pinE, int pinF, int pinG) {
  digitalWrite(commonPin, LOW); // 共通アノードをONにする
  
  // 各セグメントの状態を設定
  digitalWrite(pinA, bitRead(digitPatterns[digit], 0));
  digitalWrite(pinB, bitRead(digitPatterns[digit], 1));
  digitalWrite(pinC, bitRead(digitPatterns[digit], 2));
  digitalWrite(pinD, bitRead(digitPatterns[digit], 3));
  digitalWrite(pinE, bitRead(digitPatterns[digit], 4));
  digitalWrite(pinF, bitRead(digitPatterns[digit], 5));
  digitalWrite(pinG, bitRead(digitPatterns[digit], 6));
  
  delay(1); // 表示の安定化
  
  digitalWrite(commonPin, HIGH); // 共通アノードをOFFにする
}
