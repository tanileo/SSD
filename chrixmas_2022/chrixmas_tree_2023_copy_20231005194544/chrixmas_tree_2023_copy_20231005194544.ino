#define ANALOG_RANGE 500
#define PATTERN_PIN 15
#define SLEEP_TIME 30000

//グローバル変数
int past_time, current_time;
int current_pattern = 1;
int px[3]={12, 5, 13};   // LED anode側：左からの並び
int py[2]={4, 16};       // LED cathode側：上からの並び

//プロトタイプ宣言
void pattern_change(void);
void h_create(int n);


void setup() {
  Serial.begin(115200);
  //GPIO設定
  for(i=0; i<2; i++){
    pinMode(py[i], OUTPUT);
    digitalWrite(py[i], HIGH);
  }
  //PWM設定
  analogWriteFreq(1000);
  analogWriteRange(ANALOG_RANGE);
  //ボタンの設定
  pinMode(PATTERN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PATTERN), pattern_change, RISING);
  //時間の設定（オートスリープ用）
  past_time = millis();
  current_time = past_time;

}

void loop() {
  if(current_time - past_time < SLEEP_TIME){ //秒数が30秒以内の場合イルミネーションを光らせる
    h_create();
  }else{　//秒数が30秒を超えたらイルミネーションの光をOFFにする
    //照度センサのモニタリングの開始
    if(){
      //past_timeを現在の時間に修正
      past_time = millis();
    }

    //赤外線センサのモニタリングの開始
    if(){
      //past_timeを現在の時間に修正
      past_time = millis();
    }
  }
  //現在の秒数の取得
  current_time = millis();
}
