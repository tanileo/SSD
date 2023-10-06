/*
 * loop内で無限ループすると、WDTが働く。基本的にしない。
 * stn_time = millis();とbreak　入替。
 * game.h を作った。
 * 電子音を付けた。
 * PacManの描画を大幅修正
*/
#include <math.h>
#include <Wire.h>
#include "Adafruit_MCP23X17.h"
#include "SparkFun_MMA8452Q.h"
// -----
#include "game.h"
// -----
Adafruit_MCP23X17 mcp;
MMA8452Q accel;

#define SP 27
#define SW_A 14 // GPIO14：右 jump
#define SW_B 12 // GPIO12：左 go:Game-start
#define TOGGLE_B 19 // トグルSW：右

/*
0  □ □ □ □ □ □ □ □
1  □ □ □ □ □ □ □ □
2  □ □ □ □ □ □ □ □
3  □ □ □ □ □ □ □ □
4  □ □ □ □ □ □ □ □　　　　 ------
5  □ □ ■ □ □ □ □ ■      [       ]
6  □ □ ■ □ □ □ ■ ■      [       ]
7  ■ ■ ■ ■ ■ ■ ■ ■　　　　 ------
  0:光らない, 1:緑, 2:赤
*/

#define LEDMATRIX_ROW 8  // LEDマトリクスの行:Y
#define LEDMATRIX_COL 8  // LEDマトリクスの列:X
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

void block(void);
void create(void);
void jump(void);
void gravity(void);
void go(void);
void light(int all_block[][8]);
void light_roll(int all_block[][63]);

void GameStart(void);
void GameOver(void);

void setup_mario(void);
void setup_pacman(void);
void loop_mario(void);
void loop_pacman(void);

int i, j, k, l;

bool go_btn_EN = 0;   // SW_B:go ゲームスタートボタン 割り込み　0：無効　1：有効
bool go_btn = 0;      // SW_B:go ゲームスタート　0：準備中　1：スタート
int stn_time;
int time_service;
int current_game;
int gravity_key=0;

// 2つ目が0：ESP32のGPIO、1：MCP23017のGPIO
uint8_t pinN_plus[8][2] =  {{10, 1}, {13, 1}, {15, 0}, {4, 0}, {5, 1}, {2, 1}, {26, 0}, {32, 0}};     // アノード:Y
uint8_t pinN_mainus[2][8][2] = {{{9, 1}, {12, 1}, {15, 1}, {0, 0}, {6, 1}, {3, 1}, {0, 1}, {33, 0}},  // 赤:R1-R8, カソード:X
                              {{8, 1}, {11, 1}, {14, 1}, {2, 0}, {7, 1}, {4, 1}, {1, 1}, {25, 0}}};   // 緑:G1-G8, カソード:X

int user_position = 6;
int over_position = 6;
int past_user_position = 6;
int past_past_user_position = 6;
int quick_position = 0;
int game_over_key = 0;

int current_block[8] = {0, 0, 0, 0, 0, 0, 0, 0};

// Y, X. 0:光らない, 1:赤, 2:緑
int all_block[8][8]={{0,0,0,0,0,0,0,0},       // 1
                     {0,0,0,0,0,0,0,0},       // 2
                     {0,0,0,0,0,0,0,0},       // 3
                     {0,0,0,0,0,0,0,0},       // 4
                     {0,0,0,0,0,0,0,0},       // 5
                     {0,0,2,0,0,0,0,0},       // 6
                     {0,0,2,0,0,0,0,0},       // 7
                     {1,1,1,1,1,1,1,1}};      // 8

/* Pac Man*/
float angleX, angleY;  //現在の角度
typedef uint8_t ledMatVram; //LEDマトリクス表示用RAM
int8_t ownCoordinate[2], enemyCoordinate[2][2];  // 自機と敵機の座標, 1つ目:col:y, 2つ目:row:x
int8_t ownCoordinatePre[2], enemyCoordinatePre[2][2];  // 自機と敵機の前回の座標, 1つ目:col:y, 2つ目:row:x

hw_timer_t * timerOwn = NULL;
hw_timer_t * timerEne = NULL;

bool game = 1;  // ゲームスタート　0：スタート　1：ゲームオーバー
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

void setup(){
  Serial.begin(115200);
  pinMode(TOGGLE_B, INPUT_PULLUP);      // トグルSW：右

  //スピーカー
  pinMode(SP, OUTPUT);
  //ボタン
  pinMode(SW_A, INPUT_PULLUP);          // GPIO14：右SW
  pinMode(SW_B, INPUT_PULLUP);          // GPIO12：左SW
  attachInterrupt(SW_A, jump, FALLING); // GPIO14：右ボタン　ジャンプ
  attachInterrupt(SW_B, go, FALLING);   // GPIO12：左ボタン　ゲームスタート
  // go_btn_EN = 1;    // SW_B:go ゲームスタートボタン 割り込み　0：無効　1：有効
  
  Wire.begin();
  if (accel.begin() == false) {
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

  while(1){
    GameStart();
    if(game == 0){  // ゲームスタート　0：スタート　1：ゲームオーバー
      stn_time = millis();
      break;
    }
  }

  current_game = digitalRead(TOGGLE_B); // トグルSW：右
  Serial.println(current_game);
  
  if(current_game){ // 1: mario
    setup_mario();
  }else{
    setup_pacman();
  }
  // detachInterrupt(SW_A);  // GPIO14：右ボタン　ジャンプ
  detachInterrupt(SW_B);  // GPIO12：左ボタン　ゲームスタート
  // go_btn_EN = 0;    // SW_B:go ゲームスタートボタン 割り込み　0：無効　1：有効

  // Game Start音
  gStart();
}

void loop(){
  if(current_game){
    loop_mario();
  }else{
    loop_pacman();
  }  
}

void setup_pacman() {
  detachInterrupt(SW_A);  // GPIO14：右ボタン　ジャンプ　割り込む無効

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

void loop_pacman() {
  if(game == 0){
    getAngle(); //角度を取得

    if(OwnMoveF == 1){  // タイマー処理にて値が変化
      OwnMove();
      OwnMoveF = 0; 
    }
    if(EneMoveF == 1){  // タイマー処理にて値が変化
      EneMove();
      EneMoveF = 0;
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
    if(go_btn_EN == 0){
      // go_btn_EN = 1;
      // attachInterrupt(SW_B, go, FALLING);   // GPIO12：左ボタン　移動
    }
    drawVram();
    gOver();    // ゲームオーバー音
    delay(500);
    GameOver();
  }
}

void setup_mario() {
  ;
}
void loop_mario() {
  int i;
  block();
  create();

  for(i=0; i < 15-time_service; i++){
    // Serial.print(game_over_key);Serial.print(":");
    // Serial.println(time_service);
    light(all_block);
    if(game_over_key == 1){
      gOver();    // ゲームオーバー音
      delay(500);
      GameOver();
    }
  }
/*
  create();
  for(i=0; i<15-time_service; i++){
    light(all_block);
    if(game_over_key == 1){
      delay(500);      
      GameOver();
    }
  }*/

  if(user_position < over_position){
    gravity();
  }
}
// ----
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
  int i, j, k;
  for(i=0; i<LEDMATRIX_ROW; i++){ // Y
    // ROW(Anode:Y)側 High 点灯
    if(pinN_plus[i][1]){
      mcp.digitalWrite(pinN_plus[i][0], 1);
    }else{
      digitalWrite(pinN_plus[i][0], 1);
    }
    for(k = 0; k < 2; k++){
      if(vram[k][i] != 0){
        for(j=0; j<LEDMATRIX_COL; j++){ // COL(Cathode:X)側のチェック・反映
          if(pinN_mainus[k][j][1]){     // COL(Cathode:X)側をHigh or Lowに
            mcp.digitalWrite(pinN_mainus[k][j][0], !(vram[k][i] & (0b00000001 << j)));
          }else{
            digitalWrite(pinN_mainus[k][j][0], !(vram[k][i] & (0b00000001 << j)));
          } 
        }
      }
    }
    delay(1);  //指定時間待機

    // ROW(Anode:Y)側 Low 消灯
    if(pinN_plus[i][1]){
      mcp.digitalWrite(pinN_plus[i][0], 0);
    }else{
      digitalWrite(pinN_plus[i][0], 0);
    }
    for(k = 0; k < 2; k++){       
      for(j=0; j<LEDMATRIX_COL; j++){ // COL(Cathode:X)側のチェック・反映
        if(vram[k][i] != 0){
          if(pinN_mainus[k][j][1]){     // COL(Cathode:X)側をHighに
            mcp.digitalWrite(pinN_mainus[k][j][0], 1);
          }else{
            digitalWrite(pinN_mainus[k][j][0], 1);
          }
        }
      }
    }
    delayMicroseconds(DELAY_TIME);  //指定時間待機
  }
  // delayMicroseconds(DELAY_TIME);  //指定時間待機
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
  int i;
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

// ----
void block(void){ 
  int randomNumber1 = random(3);
  int randomNumber2 = random(3);

  // ブロックを1つ左へ移動
  for(i=0; i<7; i++){   // 修正：7
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
  // user欄　全消去
  for(i=0; i<7; i++){
    all_block[i][2]=0;    // Y:i, X:3
  }
  
  // 障害物ブロック. 0:光らない, 1:赤, 2:緑
  for(i=0; i<8; i++){
    if(current_block[i]==0){
      all_block[5][i]=0;      // Y:6, X:i
      all_block[6][i]=0;      // Y:7, X:i
    }else if(current_block[i]==1){
      all_block[5][i]=0;      // Y:6, X:i
      all_block[6][i]=1;      // Y:7, X:i
    }else{
      all_block[5][i]=1;      // Y:6, X:i
      all_block[6][i]=1;      // Y:7, X:i
    }
  }
  // over_position:6
  over_position = 6 - current_block[2];
  
  // userブロック:user_position:6
  all_block[user_position][2]+=2;       // Y:7, X:3
  all_block[user_position-1][2]+=2;     // Y:6, X:3

  for(i=0; i<7; i++){
    if(all_block[i][2]==3){ // Y:i, X:3
      game_over_key = 1;
      break;
    }
  }  

  time_service = stn_time/10000;
  if(time_service > 25){
    time_service=25;
  }
}

void jump(void){
  //Serial.println(SW_A);
  detachInterrupt(SW_A);  
  if((over_position-user_position)<2){
    user_position--;
  }
  tone(SP, 220, 100);    
  attachInterrupt(SW_A, jump, FALLING);
}
void go(void){
  detachInterrupt(digitalPinToInterrupt(SW_B)); // 割り込み停止
  tone(SP, 880, 50);
  Serial.println("go");
  // attachInterrupt(digitalPinToInterrupt(SW_B), go, FALLING);
  // go_btn_EN = 0;  // SW_B:go ゲームスタートボタン 割り込み　0：無効　1：有効
  game = 0;       // Game Start OK  
}

void gravity(void){
  //Serial.println(SW_A);
  if(gravity_key==1){
    user_position++;
    gravity_key=0;
  }else{
    gravity_key++;
  }    
}

void light(int all_block[][8]){
  for(i=0; i<8; i++){   // Y
    // Y:Anode On.
    // 光る
    if(pinN_plus[i][1]){
      mcp.digitalWrite(pinN_plus[i][0], HIGH);
    }else{
      digitalWrite(pinN_plus[i][0], HIGH);
    }
    for(j=0; j<8; j++){ // X
      // Serial.print(all_block[i][j]);
      switch(all_block[i][j]){  // Y, X. 色: 0:光らない, 1:赤, 2:緑
        case 0: break;
        case 1: if(pinN_mainus[0][j][1]){     // 赤:Cathode
                  mcp.digitalWrite(pinN_mainus[0][j][0], LOW);
                }else{
                  digitalWrite(pinN_mainus[0][j][0], LOW);
                }
                break;
        case 2: if(pinN_mainus[1][j][1]){     // 緑
                  mcp.digitalWrite(pinN_mainus[1][j][0], LOW);
                }else{
                  digitalWrite(pinN_mainus[1][j][0], LOW);
                }
                break;
      }
    }
    delay(1);
    // 消える
    // Y:Anode Off.
    if(pinN_plus[i][1]){
      mcp.digitalWrite(pinN_plus[i][0], 0);
    }else{
      digitalWrite(pinN_plus[i][0], 0);
    }    
    for(j = 0; j < 8; j++){
      switch(all_block[i][j]){  // Y, X. 色: 0:光らない, 1:赤, 2:緑
        case 0: break;
        case 1: if(pinN_mainus[0][j][1]){
                  mcp.digitalWrite(pinN_mainus[0][j][0], HIGH);
                }else{
                  digitalWrite(pinN_mainus[0][j][0], HIGH);
                }
                break;
        case 2: if(pinN_mainus[1][j][1]){
                  mcp.digitalWrite(pinN_mainus[1][j][0], HIGH);
                }else{
                  digitalWrite(pinN_mainus[1][j][0], HIGH);
                }
                break;
      }
      // delayMicroseconds(100);
    }
  }
  // delayMicroseconds(10);
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