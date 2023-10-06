#include <Adafruit_MCP23X17.h>
#include "SparkFun_MMA8452Q.h"
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
Adafruit_MCP23X17 mcp;
MMA8452Q accel;
int current_game;

// mario_run
#define SP 27
#define SW_A 14
#define SW_B 12
#define TOGGLE_SW 19

/*
  □ □ □ □ □ □ □ □
  □ □ □ □ □ □ □ □
  □ □ □ □ □ □ □ □
  □ □ □ □ □ □ □ □
  □ □ □ □ □ □ □ □　　　　 ------
  □ □ ■ □ □ □ □ ■      [       ]
  □ □ ■ □ □ □ ■ ■      [       ]
  ■ ■ ■ ■ ■ ■ ■ ■　　　　 ------
  0:光らない, 1:緑, 2:赤
*/

int go_btn = 0;
int stn_time;
int time_service;
int pinN_mainus[2][8][2]={{{9, 1}, {12, 1}, {15, 1}, {0, 0}, {6, 1}, {3, 1}, {0, 1}, {33, 0}}, {{8, 1}, {11, 1}, {14, 1}, {2, 0}, {7, 1}, {4, 1}, {1, 1}, {25, 0}}};
int pinN_plus[8][2]={{10, 1}, {13, 1}, {15, 0}, {4, 0}, {5, 1}, {2, 1}, {26, 0}, {32, 0}};
int user_position = 6;
int over_position = 6;
int past_user_position = 6;
int past_past_user_position = 6;
int quick_position = 0;
int game_over_key = 0;
int current_block[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int all_block[8][8]={{0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0},
                    {0,0,2,0,0,0,0,0},
                    {0,0,2,0,0,0,0,0},
                    {1,1,1,1,1,1,1,1}};
int sound_data1[2][3]={{440, 1, 1}, {600, 1,1}};

// pacman
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
  

void block(void){ 
  int randomNumber1 = random(3);
  int randomNumber2 = random(3);
  int i,j;

  for(i=0; i<8; i++){
    j = i + 1;
    current_block[i] = current_block[j];
  }
  if(randomNumber2==0){
    current_block[7] = randomNumber1;
  }else{
    current_block[7] = 0;
  }
  
}


void create(void){
  int i;
//user欄　全消去
  for(i=0; i<7; i++){
    all_block[i][2]=0;
  }
  
//障害物ブロック
  for(i=0; i<8; i++){
    if(current_block[i]==0){
      all_block[5][i]=0;
      all_block[6][i]=0;
    }else if(current_block[i]==1){
      all_block[5][i]=0;
      all_block[6][i]=1;
    }else{
      all_block[5][i]=1;
      all_block[6][i]=1;
    }
  }
  over_position = 6 - current_block[2];
  
//userブロック

  
  all_block[user_position][2]+=2;
  all_block[user_position-1][2]+=2;

  for(i=0; i<7; i++){
    if(all_block[i][2]==3){
      game_over_key = 1;
      break;
    }
  }


  

  time_service = stn_time/10000;
  if(time_service > 25){
    time_service=25;
  }
  
  /*if((over_position - user_position)>0 && (user_position - past_past_user_position)>0){
    all_block[user_position-1][2]+=2; all_block[user_position][2]-=2;
  }else if((user_position <= over_position)){
    game_over_key = 1;
  }

  past_past_user_position = past_user_position;
  past_user_position = user_position;

  
  for(i=5; i<7; i++){
      if(all_block[2][i]==3){
        GameOver();
      }
  }

  user_position-=(user_position-current_block[5]);
*/
}

void jump(void){
  //Serial.println(SW_A);
  if((over_position-user_position)<2){
    //sound(sound_data1, 2);
    user_position--;
  }
}
    
void gravity(void){
  //Serial.println(SW_A);
    user_position++;
}

void go(void){
  //Serial.println(SW_B);
  go_btn = 1;
}

void light(int all_block[][8]){
  int i,j;
  for(i=0; i<8; i++){
    for(j=0; j<8; j++){
      switch(all_block[i][j]){
        case 0: break;
        case 1: if(pinN_mainus[0][j][1]){
                  mcp.digitalWrite(pinN_mainus[0][j][0],LOW);
                }else{
                  digitalWrite(pinN_mainus[0][j][0],LOW);
                }
                if(pinN_plus[i][1]){
                  mcp.digitalWrite(pinN_plus[i][0],HIGH);
                }else{
                  digitalWrite(pinN_plus[i][0],HIGH);
                }break;
        case 2: if(pinN_mainus[1][j][1]){
                  mcp.digitalWrite(pinN_mainus[1][j][0],LOW);
                }else{
                  digitalWrite(pinN_mainus[1][j][0],LOW);
                }
                if(pinN_plus[i][1]){
                  mcp.digitalWrite(pinN_plus[i][0],HIGH);
                }else{
                  digitalWrite(pinN_plus[i][0],HIGH);
                }break;
      }
      switch(all_block[i][j]){
        case 0: break;
        case 1: if(pinN_mainus[0][j][1]){
                  mcp.digitalWrite(pinN_mainus[0][j][0],HIGH);
                }else{
                  digitalWrite(pinN_mainus[0][j][0],HIGH);
                }
                if(pinN_plus[i][1]){
                  mcp.digitalWrite(pinN_plus[i][0],LOW);
                }else{
                  digitalWrite(pinN_plus[i][0],LOW);
                }break;
        case 2: if(pinN_mainus[1][j][1]){
                  mcp.digitalWrite(pinN_mainus[1][j][0],HIGH);
                }else{
                  digitalWrite(pinN_mainus[1][j][0],HIGH);
                }
                if(pinN_plus[i][1]){
                  mcp.digitalWrite(pinN_plus[i][0],LOW);
                }else{
                  digitalWrite(pinN_plus[i][0],LOW);
                }break;
      }
    }
  }
  delayMicroseconds(10);
  
}


void light_roll(int all_block[][63]){
  int i,j,k,l;
  for(i=0; i<55; i++){
    for(k=0; k<10; k++){
      for(l=0; l<8; l++){
        for(j=0; j<8; j++){
          switch(all_block[l][i+j]){
            case 0: break;
            case 1: if(pinN_mainus[0][j][1]){
                      mcp.digitalWrite(pinN_mainus[0][j][0],LOW);
                    }else{
                      digitalWrite(pinN_mainus[0][j][0],LOW);
                    }
                    if(pinN_plus[i][1]){
                      mcp.digitalWrite(pinN_plus[i][0],HIGH);
                    }else{
                      digitalWrite(pinN_plus[i][0],HIGH);
                    }break;
            case 2: if(pinN_mainus[1][j][1]){
                      mcp.digitalWrite(pinN_mainus[1][j][0],LOW);
                    }else{
                      digitalWrite(pinN_mainus[1][j][0],LOW);
                    }
                    if(pinN_plus[i][1]){
                      mcp.digitalWrite(pinN_plus[i][0],HIGH);
                    }else{
                      digitalWrite(pinN_plus[i][0],HIGH);
                    }break;
              }
          switch(all_block[l][i+j]){
            case 0: break;
            case 1: if(pinN_mainus[0][j][1]){
                      mcp.digitalWrite(pinN_mainus[0][j][0],HIGH);
                    }else{
                      digitalWrite(pinN_mainus[0][j][0],HIGH);
                    }
                    if(pinN_plus[i][1]){
                      mcp.digitalWrite(pinN_plus[i][0],LOW);
                    }else{
                      digitalWrite(pinN_plus[i][0],LOW);
                    }break;
            case 2: if(pinN_mainus[1][j][1]){
                      mcp.digitalWrite(pinN_mainus[1][j][0],HIGH);
                    }else{
                      digitalWrite(pinN_mainus[1][j][0],HIGH);
                    }
                    if(pinN_plus[i][1]){
                      mcp.digitalWrite(pinN_plus[i][0],LOW);
                    }else{
                      digitalWrite(pinN_plus[i][0],LOW);
                    }break;
           }
        }
      }
      delayMicroseconds(10);
    }
  }
  
}

void GameStart(void){
  int all_block_s[8][8]={{1,0,0,0,0,0,0,1},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0},
                         {0,0,0,0,0,0,0,0}};
  int i,j,k,l;
  for(i=7; i>=0; i--){
    for(j=7; j>=0; j--){
      for(k=0; k<8; k++){
        //all_block[k][i]=0;
      }
      //all_block[j][i]=1;
      for(l=0; l<10; l++){
        light(all_block_s);
      }
    }
  }
}

void GameOver(void){

  int i;                 //1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3
  int all_block_o[8][63]={{0,0,2,2,2,0,0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0},
                          {0,2,0,0,0,2,0,0,0,0,2,0,2,0,0,0,2,2,0,0,0,0,2,2,0,2,2,2,2,2,2,0,2,0,0,0,2,0,0,0,2,0,0,0,0,0,2,0,0,2,2,2,2,2,2,0,2,0,0,0,0,2,0},
                          {2,0,0,0,0,0,2,0,0,0,2,0,2,2,0,0,2,0,2,0,0,2,0,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,0,2,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2,0},
                          {2,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,2,0,2,0,0,2,0,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2,0},
                          {2,0,0,2,2,2,2,0,0,2,0,0,0,2,0,0,2,0,0,2,2,0,0,2,0,2,2,2,2,2,0,2,0,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,2,2,2,2,0,0,2,2,2,2,2,0,0},
                          {2,0,0,0,0,0,2,0,2,2,2,2,2,2,2,0,2,0,0,0,0,0,0,2,0,2,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,0,2,0,2,0,0,0,0,2,0,0,0,0,0,0,2,0,2,0,0,0,0},
                          {0,2,0,0,0,2,0,0,2,0,0,0,0,0,2,0,2,0,0,0,0,0,0,2,0,2,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,2,0,2,0,0,0,0,2,0,0,0,0,0,0,2,0,0,2,0,0,0},
                          {0,0,2,2,2,0,0,0,2,0,0,0,0,0,2,0,2,0,0,0,0,0,0,2,0,2,2,2,2,2,2,0,0,2,2,2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,2,2,2,2,2,0,2,0,0,0,2,0,0}}; 

  for(i=0; i<100; i++){
    light_roll(all_block_o);
  }
}

/*void sound(int sound_data[][3], int n){
  int i;
  for(i=0; i<n; i++){
    tone(SP, sound_data[0][i]);
    delay(sound_data[i][2]);
  }
}*/

void setup() {
  pinMode(TOGGLE_SW, INPUT);
  current_game=digitalRead(TOGGLE_SW);
  Serial.write(current_game);
  if(current_game){
    setup_mario();
  }else{
    setup_pacman();
  }
}

void setup_mario(){
    Serial.begin(9600);
  Wire.begin();
  int i,j;
  mcp.begin_I2C();

  //スピーカー
  pinMode(SP, OUTPUT);
  //ボタン
  pinMode(SW_A, INPUT_PULLUP);
  pinMode(SW_B, INPUT_PULLUP);
  attachInterrupt(SW_A, jump, FALLING);
  attachInterrupt(SW_B, go, FALLING);
  //エキスパンダ（LED　カソード）
  for (i = 0; i < 2; i++) {
    for(j = 0; j < 8; j++){
      if(pinN_mainus[i][j][1]){
        mcp.pinMode(pinN_mainus[i][j][0], OUTPUT);
        mcp.digitalWrite(pinN_mainus[i][j][0], HIGH);
      }else{
        pinMode(pinN_mainus[i][j][0], OUTPUT);
        digitalWrite(pinN_mainus[i][j][0], HIGH);
      }
    }
  }
  //マイコン（LED　アノード）
  for(i=0; i<8; i++){
    if(pinN_plus[i][1]){
      mcp.pinMode(pinN_plus[i][0], OUTPUT);
      mcp.digitalWrite(pinN_plus[i][0], LOW);
    }else{
      pinMode(pinN_plus[i][0], OUTPUT);
      digitalWrite(pinN_plus[i][0], LOW);
    }
  }
  while(1){
    GameStart();
    if(go_btn==1){
      break;
      stn_time = millis();
    }
  }
}

void setup_pacman(){
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

void mario() {
  // put your main code here, to run repeatedly:
  int i;
  block();
  create();
  for(i=0; i<15-time_service; i++){
    light(all_block);
    if(game_over_key == 1){
      GameOver();
    }
  }
  create();
  for(i=0; i<15-time_service; i++){
    light(all_block);
    if(game_over_key == 1){
      GameOver();
    }
  }
  if(user_position < over_position){
    //gravity();
  }
}

void pacman(){
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

void loop(){
  if(current_game){
    mario();
  }else{
    pacman();
  }
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