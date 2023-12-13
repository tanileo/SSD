const int photoResistorPin = 28;  // フォトトランジスタのピン
const int slidePin = 22;  // フォトトランジスタ　オンオフのボタン
const int pattern_pin = 19;  // フォトトランジスタ　オンオフのボタン
const int ledPins[] = {2, 29, 14, 15, 17, 16};  // LEDのピン
const int fadeInTime = 150000;  // フェードインの時間 (0 から 4095)
const int fadeOutTime = 150000;  // フェードアウトの時間 (0 から 4095)
const int fadeInDelay = 1000;  // フェードインの遅延時間
const int fadeOutDelay = 1000;  // フェードアウトの遅延時間
const int pattern_num1 = 6;
const int pattern_num2 = 6;
const int pattern_num = 2;
int current_pattern = 0;
int sensorValue;
int maxBrightness;
int pattern[pattern_num][pattern_num1][pattern_num2] = {{{1, 0, 0, 0, 0, 0}, {0, 1, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0}, {0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 1}},
                                                        {{1, 1, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0}, {0, 0, 1, 1, 0, 0}, {0, 0, 0, 1, 1, 0}, {0, 0, 0, 0, 1, 1}, {1, 0, 0, 0, 0, 1}}};


void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // 12ビットのPWM分解能を設定
  analogWriteResolution(12);
  pinMode(pattern_pin, INPUT_PULLUP);
  pinMode(slidePin, INPUT_PULLUP);
  attachInterrupt(pattern_pin, change_pattern, FALLING);
}

void loop() {
  judge_photo();

  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print(" - Max Brightness: ");
  Serial.println(maxBrightness);

  fadeIn();  // フェードイン
  // delay(fadeInDelay);

  // fadeOut();  // フェードアウト
  // delay(fadeOutDelay);
}

void fadeIn() {
  for (int i = 0; i < 6; i++){
    for (int brightness = 0; brightness <= maxBrightness; brightness++) {
      int stepDelay = fadeInTime / (maxBrightness + 1);  // 各ステップごとの待ち時間
      for (int j = 0; j < 6; j++) {
        if(pattern[current_pattern][j][i]){
          analogWrite(ledPins[j], brightness);
        }
        delayMicroseconds(stepDelay);
      }
    }
    for (int brightness = maxBrightness; brightness >= 0; brightness--) {
      int stepDelay = fadeOutTime / (maxBrightness + 1);  // 各ステップごとの待ち時間
      for (int j = 0; j < 6; j++) {
        if(pattern[current_pattern][j][i]){
          analogWrite(ledPins[j], brightness);
        }
      }
      delayMicroseconds(stepDelay);
      judge_photo();
    }
  }
}

void fadeOut() {
  for (int i = 0; i < 6; i++){
    for (int brightness = maxBrightness; brightness >= 0; brightness--) {
      int stepDelay = fadeOutTime / (maxBrightness + 1);  // 各ステップごとの待ち時間
      for (int j = 0; j < 6; j++) {
        if(pattern[current_pattern][i][j]){
          analogWrite(ledPins[j], brightness);
        }
        delayMicroseconds(stepDelay);
        judge_photo();
      }
    }
  }
}

void judge_photo(void){
  int slide = analogRead(slidePin);
  if(slide == 0){
    sensorValue = analogRead(photoResistorPin);  // フォトトランジスタの値を読み取る
    maxBrightness = map(sensorValue, 0, 1023, 0, 4095);  // フォトトランジスタの値をLEDの最大値にマップ
  }else if(slide == 1){
    maxBrightness = 4095;
  }
}

void change_pattern(void){
  current_pattern++;
  for(int i = 0; i<6; i++){
    analogWrite(ledPins[i], 0);
  }
  if(current_pattern == pattern_num){
    current_pattern = 0;
  }
}
