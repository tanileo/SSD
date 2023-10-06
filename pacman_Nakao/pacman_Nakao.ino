#include <math.h>
#include <Wire.h>
#include <Arduino.h>
#include "Adafruit_MCP23X17.h"
#include "SparkFun_MMA8452Q.h"
Adafruit_MCP23X17 mcp;
MMA8452Q accel;

#define LEDMATRIX_ROW 8  //LEDマトリクスの行
#define LEDMATRIX_COL 8 //LEDマトリクスの列
#define DELAY_TIME 100
#define COLOR 2 //色の数
#define OWN 1
#define ENEMY_1 2
#define ENEMY_2 3

void getAngle(void); //角度を取得
void drawVram(void); //VRAMの内容をもとに描画
void refleCoordinateToVram(uint8_t coordinateX, uint8_t coordinateY, uint8_t ownOrEne);  //座標と自機 or 敵機を元にvramに反映
void IRAM_ATTR onTimerOwn();
void IRAM_ATTR onTimerEne();
void gameOver();

int i,game = 0;
uint16_t pinAbsROW[8] = {15,2,4,16,17,5,15,19};  //点灯時アノード,ESP32のピン番号
uint16_t pinAbsCol0[8] = {0,2,4,6,8,10,12,14}; //赤,点灯時カソード,IOエキスパンダのピン番号
uint16_t pinAbsCol1[8] = {1,3,5,7,9,11,13,15};  //黄,点灯時カソード,IOエキスパンダのピン番号
float angleX,angleY;  //現在の角度
typedef uint8_t ledMatVram; //LEDマトリクス表示用RAM
uint8_t ownCoordinate[2],enemyCoordinate[2][2];  //自機と敵機の座標,1つ目:ROW,2つ目:COL
uint8_t ownCoordinate1[2],enemyCoordinate1[2][2];  //自機と敵機の前回の座標,1つ目:ROW,2つ目:COL
hw_timer_t * timerOwn = NULL;
hw_timer_t * timerEne = NULL;

ledMatVram vram[COLOR][LEDMATRIX_COL] = {//0:赤,1:黄
{
  //RED
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
},{
  //YEL
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();

  if (accel.begin() == false) {
    //ジャイロセンサの起動に失敗
    while (1);
  }
  
  //LEDマトリクスの設定
  mcp.begin_I2C(0x01);  // MCP23017のI2Cアドレスを0x01に設定(0x00は省略可)
  for(int i=0;i<8;i++){  //ROWピンを初期化
    pinMode(pinAbsROW[i],OUTPUT);
  }
  //IOエキスパンダ初期化
  for (int i = 0; i <= 15; i++) {
    mcp.pinMode(i, OUTPUT);
  }

  //自機の初期設定(4:4)
  ownCoordinate[0] = 4;
  ownCoordinate[1] = 4;
  refleCoordinateToVram(ownCoordinate[0],ownCoordinate[1],OWN);

  //敵機1の初期設定(6,2)
  enemyCoordinate[0][0] = 6;
  enemyCoordinate[0][1] = 2;
  refleCoordinateToVram(enemyCoordinate[0][0], enemyCoordinate[0][1],ENEMY_1);
  //敵機2の初期設定(2,6)
  enemyCoordinate[1][0] = 2;
  enemyCoordinate[1][1] = 6;
  refleCoordinateToVram(enemyCoordinate[1][0], enemyCoordinate[1][1],ENEMY_2);

  //1マイクロ秒のタイマー,自機は適宜変更
  timerOwn = timerBegin(0, getApbFrequency()/1000000, true);
  timerAttachInterrupt(timerOwn, &onTimerOwn, true);
  timerAlarmWrite(timerOwn, 1000000, true);
  timerAlarmEnable(timerOwn);
  //1マイクロ秒のタイマー,敵機は1秒固定
  timerEne = timerBegin(1, getApbFrequency()/1000000, true);
  timerAttachInterrupt(timerEne, &onTimerEne, true);
  timerAlarmWrite(timerEne, 700000, true);  //700[ms]
  timerAlarmEnable(timerEne);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  while(game == 0){ //gameが0ならゲーム続行
    //前回の座標を保存
    ownCoordinate1[0] == ownCoordinate[0];
    ownCoordinate1[1] == ownCoordinate[1];
    enemyCoordinate1[0][0] = enemyCoordinate[0][0];
    enemyCoordinate1[0][1] = enemyCoordinate[0][1];
    enemyCoordinate1[1][0] = enemyCoordinate[1][0];
    enemyCoordinate1[1][1] = enemyCoordinate[1][1];

    getAngle(); //角度を取得
    //角度から自機の移動先を決定
    //X+:手前から奥,X-:奥から手前,Y+:右,Y-:左
    if(angleX <= 0){
      //行を-1
      if(ownCoordinate[1] == 0){
        //端っこなのでそのまま
      }else{
        ownCoordinate[0] --;
      }
    }else if(angleX > 0){
      //行を+1
      if(ownCoordinate[1] == 7){
        //端っこなのでそのまま
      }else{
        ownCoordinate[0] ++;
      }
    }

    if(angleY <= 0){
      //列を-1
      if(ownCoordinate[0] == 0){
        //端っこなのでそのまま
      }else{
        ownCoordinate[1] --;
      }
    }else if(angleX > 0){
      //列を+1
      if(ownCoordinate[0] == 7){
        //端っこなのでそのまま
      }else{
        ownCoordinate[1] ++;
      }
    }
    //角度に応じて移動速度を(何秒後に反映させるか)決定
    int r = sqrt(pow(angleX,2) + pow(angleY,2));
    int ms = ((95 - r) * 10) + 10;
    timerAlarmWrite(timerOwn, (ms * 1000), true);

    //敵機の移動先を決定
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

    //ゲームオーバー判定
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
  }

  //ゲームオーバー

  drawVram(); //描画
}

void getAngle(void)
{
  if (accel.available()) {
    float ax = accel.getCalculatedX();
    float ay = accel.getCalculatedY();
    float az = accel.getCalculatedZ();
    angleX = atan2(ax, az) * 180.0 / PI;  //X軸の角度を算出
    angleY = atan2(ay, az) * 180.0 / PI;  //Y軸の角度を算出
  }
}

void drawVram(void)
{
  int i,j;
  for(i=0;i<LEDMATRIX_COL;i++){
    //1つ目の色
    for(j=0;j<LEDMATRIX_ROW;j++){   //ROW側のチェック・反映
      digitalWrite(pinAbsROW[j],vram[0][i] & (0b00000001 << j));
    }
    mcp.digitalWrite(pinAbsCol0[j],LOW);  //COL側を点灯に
    delayMicroseconds(DELAY_TIME);  //指定時間待機
    for(j=0;j<LEDMATRIX_ROW;j++){   //ROW側の初期化
      digitalWrite(pinAbsROW[j],LOW);
    }
    mcp.digitalWrite(pinAbsCol0[i],HIGH);  //COL側の初期化
    
    //2つ目の色
    for(j=0;j<LEDMATRIX_ROW;j++){   //ROW側のチェック・反映
      digitalWrite(pinAbsROW[j],vram[1][i] & (0b00000001 << j));
    }
    mcp.digitalWrite(pinAbsCol0[j],LOW);  //COL側を点灯に
    delayMicroseconds(DELAY_TIME);  //指定時間待機
    for(j=0;j<LEDMATRIX_ROW;j++){   //ROW側の初期化
      digitalWrite(pinAbsROW[j],LOW);
    }
    mcp.digitalWrite(pinAbsCol0[i],HIGH);  //COL側の初期化
  }
}

void refleCoordinateToVram(uint8_t coordinateX, uint8_t coordinateY, uint8_t ownOrEne)
{
  //現在の座標を削除
  uint8_t a;
  if(ownOrEne == OWN){
    a = 0b00000001 << (ownCoordinate1[0] - 1);
    a = ~a;
    if(ownOrEne == OWN){
      //自機の場合
      vram[1][ownCoordinate1[0]] & a;
    }else if((ownOrEne == ENEMY_1) || (ownOrEne == ENEMY_2)){
      //敵機の場合
      vram[0][ownCoordinate1[0]] & a;
    }
  }else if(ownOrEne == ENEMY_1){
    a = 0b00000001 << (enemyCoordinate1[0][0] - 1);
    a = ~a;
    if(ownOrEne == OWN){
      //自機の場合
      vram[1][ownCoordinate1[0]] & a;
    }else if((ownOrEne == ENEMY_1) || (ownOrEne == ENEMY_2)){
      //敵機の場合
      vram[0][enemyCoordinate1[0][0]] & a;
    }
  }else if(ownOrEne == ENEMY_2){
    a = 0b00000001 << (enemyCoordinate1[1][0] - 1);
    a = ~a;
    if(ownOrEne == OWN){
      //自機の場合
      vram[1][ownCoordinate1[1]] & a;
    }else if((ownOrEne == ENEMY_1) || (ownOrEne == ENEMY_2)){
      //敵機の場合
      vram[0][enemyCoordinate1[1][0]] & a;
    }
  }

  //新しい座標を設定
  uint8_t b;
  b = 0b00000001 << (coordinateY - 1);
  if(ownOrEne == OWN){
    //自機の場合
    vram[1][coordinateX] | b;
  }else if((ownOrEne == ENEMY_1) || (ownOrEne == ENEMY_2)){
    //敵機の場合
    vram[0][coordinateX] | b;
  }
}

void IRAM_ATTR onTimerOwn()
{
  //タイマー
  refleCoordinateToVram(ownCoordinate[0],ownCoordinate[1],OWN);
}

void IRAM_ATTR onTimerEne()
{
  //タイマー
  refleCoordinateToVram(enemyCoordinate[0][0], enemyCoordinate[0][1],ENEMY_1);
  refleCoordinateToVram(enemyCoordinate[1][0], enemyCoordinate[1][1],ENEMY_2);
}

