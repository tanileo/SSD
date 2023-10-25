/*
 * GPIOピンの名称、データ、出力設定修正：谷君のPGMに合わした。
   pinAbsROW -> pinN_plus, pinAbsCol0,1 -> pinN_mainus
 * refleCoordinateToVram
   &=：代入演算子, xyの混乱，-1
 * ownCoordinate, enemyCoordinate: uint8 ⇒符号ありに
 * 角度から座標の増減処理を修正
 * 表示の処理をかなり変更
*/
#include <math.h>
#include <Wire.h>
// #include <Arduino.h>
#include "Adafruit_MCP23X17.h"
#include "SparkFun_MMA8452Q.h"
Adafruit_MCP23X17 mcp;
MMA8452Q accel;

#define LEDMATRIX_ROW 8  //LEDマトリクスの行:Y
#define LEDMATRIX_COL 8 //LEDマトリクスの列:X
#define DELAY_TIME 100
#define COLOR 2 //色の数
#define OWN 1
#define ENEMY_1 2
#define ENEMY_2 3

void getAngle(void); //角度を取得
void drawVram(void); //VRAMの内容をもとに描画
void refleCoordinateToVram(uint8_t coordinateX, uint8_t coordinateY, uint8_t ownOrEne);  //座標と自機 or 敵機を元にvramに反映
void OwnMove();
void EneMove();
void IRAM_ATTR onTimerOwn();
void IRAM_ATTR onTimerEne();
void gameOver();

int i, j, k, l;

// 2つ目が0：ESP32のGPIO、1：MCP23017のGPIO
uint8_t pinN_plus[8][2] =  {{10, 1}, {13, 1}, {15, 0}, {4, 0}, {5, 1}, {2, 1}, {26, 0}, {32, 0}};  // 点灯時アノード:Y
uint8_t pinN_mainus[2][8][2] = {{{9, 1}, {12, 1}, {15, 1}, {0, 0}, {6, 1}, {3, 1}, {0, 1}, {33, 0}}, // 赤,点灯時カソード:X
                              {{8, 1}, {11, 1}, {14, 1}, {2, 0}, {7, 1}, {4, 1}, {1, 1}, {25, 0}}}; // 緑,点灯時カソード:X

float angleX, angleY;  //現在の角度
typedef uint8_t ledMatVram; //LEDマトリクス表示用RAM
int8_t ownCoordinate[2], enemyCoordinate[2][2];  // 自機と敵機の座標, 1つ目:col:y, 2つ目:row:x
int8_t ownCoordinatePre[2], enemyCoordinatePre[2][2];  // 自機と敵機の前回の座標, 1つ目:col:y, 2つ目:row:x

hw_timer_t * timerOwn = NULL;
hw_timer_t * timerEne = NULL;

bool game = 0;
bool OwnMoveF = 0,   EneMoveF = 0;

ledMatVram vram[COLOR][LEDMATRIX_COL] = { // 0:赤, 1:緑 ※表示は左右逆
{
  // RED
  0b00000000,   // 1行目
  0b00000000,   // 2行目
  0b00000000,   // 3行目
  0b00000000,   // 4行目
  0b00000000,   // 5行目
  0b00000000,   // 6行目
  0b00000000,   // 7行目
  0b00000000,   // 8行目
},{
  // GREEN
  0b00000000,   // 1行目
  0b00000000,   // 2行目
  0b00000000,   // 3行目
  0b00000000,   // 4行目
  0b00000000,   // 5行目
  0b00000000,   // 6行目
  0b00000000,   // 7行目
  0b00000000,   // 8行目
  }
};

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (accel.begin() == false) {
    //ジャイロセンサの起動に失敗
    // while (1);
    Serial.println("ジャイロセンサの起動に失敗");
  }
  
  //LEDマトリクスの設定
  mcp.begin_I2C(0x20);  // MCP23017のI2Cアドレスを0x20に設定

  // LED　アノード
  for(i=0; i<8; i++){
    if(pinN_plus[i][1]){    // MCP2307用：10, 13, 5, 2
      mcp.pinMode(pinN_plus[i][0], OUTPUT);
      mcp.digitalWrite(pinN_plus[i][0], LOW);
    }else{                  // ESP32用：15, 4, 26, 32
      pinMode(pinN_plus[i][0], OUTPUT);
      digitalWrite(pinN_plus[i][0], LOW);
    }
  }
  // LED　カソード
  for (i = 0; i < 2; i++) {   // 0:Red, 1:Green
    for(j = 0; j < 8; j++){
      if(pinN_mainus[i][j][1]){   // MCP2307用：9, 12, 15, 6, 3, 0, 8, 11, 14, 7, 4, 1
        mcp.pinMode(pinN_mainus[i][j][0], OUTPUT);
        mcp.digitalWrite(pinN_mainus[i][j][0], HIGH);
      }else{                      // ESP32用：0, 33, 2, 25
        pinMode(pinN_mainus[i][j][0], OUTPUT);
        digitalWrite(pinN_mainus[i][j][0], HIGH);
      }
    }
  }

  // 自機の初期設定(4:4)
  ownCoordinate[0] = 4;     // Y
  ownCoordinate[1] = 4;     // X
  ownCoordinatePre[0] = ownCoordinate[0];
  ownCoordinatePre[1] = ownCoordinate[1];
  refleCoordinateToVram(ownCoordinate[1], ownCoordinate[0], OWN);   // 引数: Y, X

  //敵機1の初期設定(6,2)
  enemyCoordinate[0][0] = 6;    // Y
  enemyCoordinate[0][1] = 2;    // X
  enemyCoordinatePre[0][0] = enemyCoordinate[0][0];
  enemyCoordinatePre[0][1] = enemyCoordinate[0][1];  
  refleCoordinateToVram(enemyCoordinate[0][1], enemyCoordinate[0][0], ENEMY_1);   // 引数: Y, X
  
  //敵機2の初期設定(2,6)
  enemyCoordinate[1][0] = 2;    // Y
  enemyCoordinate[1][1] = 6;    // X
  enemyCoordinatePre[1][0] = enemyCoordinate[1][0];
  enemyCoordinatePre[1][1] = enemyCoordinate[1][1];    
  refleCoordinateToVram(enemyCoordinate[1][1], enemyCoordinate[1][0], ENEMY_2);   // 引数: Y, X

  //1マイクロ秒のタイマー,自機は適宜変更
  timerOwn = timerBegin(0, getApbFrequency()/1000000, true);
  timerAttachInterrupt(timerOwn, &onTimerOwn, true);
  timerAlarmWrite(timerOwn, 1000000, true);  // 1000ms
  timerAlarmEnable(timerOwn);

  //1マイクロ秒のタイマー,敵機は1秒固定
  timerEne = timerBegin(1, getApbFrequency()/1000000, true);
  timerAttachInterrupt(timerEne, &onTimerEne, true);
  timerAlarmWrite(timerEne, 700000, true);  // 700[ms]
  timerAlarmEnable(timerEne);
}

void loop() {
  // while(game == 0){ //gameが0ならゲーム続行
  if(game == 0){
    getAngle(); //角度を取得

    if(OwnMoveF == 1){  // タイマー処理にて値が変化
      OwnMove();
    }
    if(EneMoveF == 1){  // タイマー処理にて値が変化
      EneMove();
    }

    // 角度に応じて移動速度を(何秒後に反映させるか)決定
    int r = sqrt(pow(angleX, 2) + pow(angleY, 2));
    int ms = ((95 - r) * 10) + 10;
    Serial.print(ms);
    timerAlarmWrite(timerOwn, (ms * 100), true);   // *1000 ⇒ *100に変更　

    // ゲームオーバー判定
    if (enemyCoordinate[0][0] == ownCoordinate[0]){
      if(enemyCoordinate[0][1] == ownCoordinate[1]){
        game = 1; //ゲームオーバー
      }
    }
    if (enemyCoordinate[1][0] == ownCoordinate[0]){
      if(enemyCoordinate[1][1] == ownCoordinate[1]){
        game = 1; //ゲームオーバー
      }
    }
    drawVram(); //描画
  }else{
  //ゲームオーバー
    drawVram(); //描画
  }
}
// 角度取得
void getAngle(void)
{
  if (accel.available()) {
    float ax = accel.getCalculatedX();
    float ay = accel.getCalculatedY();
    float az = accel.getCalculatedZ();
    angleX = atan2(ax, -az) * 180.0 / PI;  // X軸の角度を算出
    angleY = atan2(ay, -az) * 180.0 / PI;  // Y軸の角度を算出
    // Serial.print("X: "); Serial.print(angleX);
    // Serial.print(" Y: "); Serial.println(angleY);    
  }
}
// 描画
void drawVram(void)
{
  for(i=0; i<LEDMATRIX_COL; i++){
    //1つ目の色:赤
    for(j=0; j<LEDMATRIX_ROW; j++){ // ROW(Anode:Y)側のチェック・反映
      if(pinN_plus[j][1]){
        mcp.digitalWrite(pinN_plus[j][0], vram[0][j] & (0b00000001 << i));
      }else{
        digitalWrite(pinN_plus[j][0], vram[0][j] & (0b00000001 << i));
      }
    }
    if(pinN_mainus[0][i][1]){   // COL(Cathode:X)側をLowに
      mcp.digitalWrite(pinN_mainus[0][i][0], LOW);
    }else{
      digitalWrite(pinN_mainus[0][i][0], LOW);
    }
    // delayMicroseconds(DELAY_TIME);  //指定時間待機

    for(j=0;j<LEDMATRIX_ROW;j++){   // ROW(Anode:Y)側の初期化
      if(pinN_plus[j][1]){
        mcp.digitalWrite(pinN_plus[j][0], LOW);
      }else{
        digitalWrite(pinN_plus[j][0], LOW);
      }
    } 
    if(pinN_mainus[0][i][1]){   // COL(Cathode:X)側の初期化
      //IOエキスパンダで処理
      mcp.digitalWrite(pinN_mainus[0][i][0], HIGH);
    }else{
      //ESP32で処理
      digitalWrite(pinN_mainus[0][i][0], HIGH);
    }
    // delayMicroseconds(DELAY_TIME);  //指定時間待機

    //2つ目の色:緑
    for(j=0; j<LEDMATRIX_ROW; j++){   // ROW(Anode:Y)側のチェック・反映
      if(pinN_plus[j][1]){
        mcp.digitalWrite(pinN_plus[j][0], vram[1][j] & (0b00000001 << i));
      }else{
        digitalWrite(pinN_plus[j][0], vram[1][j] & (0b00000001 << i));
      }
    }
    if(pinN_mainus[1][i][1]){  // COL(Cathode:X)側をLowに
      mcp.digitalWrite(pinN_mainus[1][i][0], LOW);
    }else{
      digitalWrite(pinN_mainus[1][i][0], LOW);
    }
    // delayMicroseconds(DELAY_TIME);  //指定時間待機

    for(j=0;j<LEDMATRIX_ROW;j++){   // ROW(Anode:Y)側の初期化
      if(pinN_plus[j][1]){
        mcp.digitalWrite(pinN_plus[j][0], LOW);
      }else{
        digitalWrite(pinN_plus[j][0], LOW);
      }
    } 
    if(pinN_mainus[1][i][1]){  // COL(Cathode:X)側の初期化
      mcp.digitalWrite(pinN_mainus[1][i][0], HIGH);
    }else{
      digitalWrite(pinN_mainus[1][i][0], HIGH);
    }
  }
}

void refleCoordinateToVram(uint8_t coordinateX, uint8_t coordinateY, uint8_t ownOrEne)
{
  // 現在の座標を削除
  uint8_t a;
  if(ownOrEne == OWN){
    a = 0b00000001 << (ownCoordinatePre[1] - 1);  // 1行分y x:横方向のデータ
    a = ~a;
    // 自機の場合
    vram[1][ownCoordinatePre[0] - 1] &= a;        // 緑．1行分y x:横方向のデータを更新 
  }else if(ownOrEne == ENEMY_1){
    a = 0b00000001 << (enemyCoordinatePre[0][1] - 1);  // 1行分y x:横方向のデータ
    a = ~a;
    // 敵機1の場合
    vram[0][enemyCoordinatePre[0][0] - 1] &= a;   // 赤．1行分y x:横方向のデータを更新
  }else if(ownOrEne == ENEMY_2){
    a = 0b00000001 << (enemyCoordinatePre[1][1] - 1);  // 1行分y x:横方向のデータ
    a = ~a;
    // 敵機2の場合
    vram[0][enemyCoordinatePre[1][0] - 1] &= a;   // 赤．1行分y x:横方向のデータを更新
  }

  // 新しい座標を設定
  uint8_t b;
  b = 0b00000001 << (coordinateX - 1);  // 1行分y x:横方向のデータ
  if(ownOrEne == OWN){
    // 自機の場合：緑
    vram[1][coordinateY - 1] |= b;
  }else if((ownOrEne == ENEMY_1) || (ownOrEne == ENEMY_2)){
    // 敵機の場合：赤
    vram[0][coordinateY - 1] |= b;
  }
}

void OwnMove()
{
  //前回の座標を保存
  ownCoordinatePre[0] = ownCoordinate[0];   // Y
  ownCoordinatePre[1] = ownCoordinate[1];   // X

  // 角度から自機の移動先を決定
  // X+:手前から奥,X-:奥から手前,Y+:右,Y-:左
  if(angleX < -10){
    //行を-1
    ownCoordinate[1] --;
    if(ownCoordinate[1] <= 1){
      ownCoordinate[1] = 1;   // 端っこなのでそのまま
    }
  }else if(angleX > 10){
    //行を+1
    ownCoordinate[1] ++;      
    if(ownCoordinate[1] >= 8){
      ownCoordinate[1] = 8;   // 端っこなのでそのまま
    }
  }

  if(angleY < -10){
    //列を-1
    ownCoordinate[0] --;      
    if(ownCoordinate[0] <= 1){
      ownCoordinate[0] = 1;   // 端っこなのでそのまま
    }
  }else if(angleY > 10){
    //列を+1
    ownCoordinate[0] ++;
    if(ownCoordinate[0] >= 8){
      ownCoordinate[0] = 8;   // 端っこなのでそのまま
    }
  }
  
  refleCoordinateToVram(ownCoordinate[1], ownCoordinate[0], OWN);

  Serial.print("GX: "); Serial.print(angleX);
  Serial.print(" GY: "); Serial.println(angleY);      
  Serial.print("X: "); Serial.print(ownCoordinate[1]);
  Serial.print(" Y: "); Serial.println(ownCoordinate[0]);
  OwnMoveF = 0;     
}
void EneMove()
{
  enemyCoordinatePre[0][0] = enemyCoordinate[0][0];
  enemyCoordinatePre[0][1] = enemyCoordinate[0][1];
  enemyCoordinatePre[1][0] = enemyCoordinate[1][0];
  enemyCoordinatePre[1][1] = enemyCoordinate[1][1];  
  // 敵機の移動先を決定
  for(i = 0;i < 2;i++){
    if(enemyCoordinate[i][0] < ownCoordinate[0]){
      enemyCoordinate[i][0] ++;
    }else if(enemyCoordinate[i][0] > ownCoordinate[0]){
      enemyCoordinate[i][0] --;
    }

    if(enemyCoordinate[i][1] < ownCoordinate[1]){
      enemyCoordinate[i][1] ++;
    }else if(enemyCoordinate[i][1] > ownCoordinate[1]){
      enemyCoordinate[i][1] --;
    }
  }
  refleCoordinateToVram(enemyCoordinate[0][1], enemyCoordinate[0][0], ENEMY_1);
  refleCoordinateToVram(enemyCoordinate[1][1], enemyCoordinate[1][0], ENEMY_2);  
  EneMoveF = 0;   
}
// 自機タイマー処理
void IRAM_ATTR onTimerOwn()
{
  OwnMoveF = 1; 
}
// 敵機タイマー処理
void IRAM_ATTR onTimerEne()
{
  EneMoveF = 1;
}