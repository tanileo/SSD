/*
 * blockのループ回数を修正
 * stn_time = millis();
 * light関数を大幅に変更
 * light_roll関数でGPIOエラーが出ている。未修正。
*/
#include <Adafruit_MCP23X17.h>
#include <Wire.h>

#define SP 27
#define SW_A 14
#define SW_B 12
#define TOGGLE_B 19

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

int go_btn = 0;
int stn_time;
int time_service;

// 2つ目が0：ESP32のGPIO、1：MCP23017のGPIO
int pinN_mainus[2][8][2]={{{9, 1}, {12, 1}, {15, 1}, {0, 0}, {6, 1}, {3, 1}, {0, 1}, {33, 0}}, {{8, 1}, {11, 1}, {14, 1}, {2, 0}, {7, 1}, {4, 1}, {1, 1}, {25, 0}}};
int pinN_plus[8][2]={{10, 1}, {13, 1}, {15, 0}, {4, 0}, {5, 1}, {2, 1}, {26, 0}, {32, 0}};

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

int sound_data1[2][3]={{440, 1, 1}, {600, 1, 1}};
  
Adafruit_MCP23X17 mcp;

void block(void);
void create(void);
void jump(void);
void gravity(void);
void go(void);
void light(int all_block[][8]);
void light_roll(int all_block[][63]);
void GameStart(void);
void GameOver(void);

int i, j;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mcp.begin_I2C(0x20);

  //スピーカー
  pinMode(SP, OUTPUT);
  //ボタン
  pinMode(SW_A, INPUT_PULLUP);          // GPIO14：右SW
  pinMode(SW_B, INPUT_PULLUP);          // GPIO12：左SW
  attachInterrupt(SW_A, jump, FALLING); // GPIO14：右ボタン　ジャンプ
  attachInterrupt(SW_B, go, FALLING);   // GPIO12：左ボタン　移動

  // エキスパンダ（LED　カソード）
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
  // マイコン（LED　アノード）
  for(i=0; i<8; i++){
    if(pinN_plus[i][1]){
      mcp.pinMode(pinN_plus[i][0], OUTPUT);
      mcp.digitalWrite(pinN_plus[i][0], LOW);
    }else{
      pinMode(pinN_plus[i][0], OUTPUT);
      digitalWrite(pinN_plus[i][0], LOW);
    }
  }

  /*
  while(1){
    GameStart();
    if(go_btn==1){
      break;
      stn_time = millis();
    }
  }*/
}

void loop() {
  int i;
  block();
  create();

  for(i=0; i < 15-time_service; i++){
    // Serial.print(game_over_key);Serial.print(":");
    // Serial.println(time_service);
    light(all_block);
    if(game_over_key == 1){
      GameOver();
    }
  }
/*
  create();
  for(i=0; i<15-time_service; i++){
    light(all_block);
    if(game_over_key == 1){
      GameOver();
    }
  }*/

  if(user_position < over_position){
    gravity();
  }
}

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