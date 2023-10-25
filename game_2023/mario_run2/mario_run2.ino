#include <Adafruit_MCP23X17.h>

#define SP 27
#define SW_A 14
#define SW_B 12
#define TOGGLE_B 19
#include <Wire.h>



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


  
 Adafruit_MCP23X17 mcp;



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
    detachInterrupt(digitalPinToInterrupt(SW_A)); // 割り込み停止
    tone(SP, 440, 100);
    Serial.println("jump");
    attachInterrupt(digitalPinToInterrupt(SW_A), jump, FALLING);
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
  int all_block_s[ANI_NUM][8][8]={{{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}},
                            
                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}}, 
                            
                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}},

                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}},
                            
                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}}, 
                            
                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}},

                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}},
                            
                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}}, 
                            
                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}},

                                  {{0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0},
                                   {0,0,0,0,0,0,0,0}},
                                   };
  
  int i,j,k,l;
  for(i=0; i<ANI_NUM; i++){
    int all_block_k[8][8];
    for(j=0; j<8; j++){
      for(k=0; k<8; k++){
        all_block_k[j][k]=all_block_s[i][j][k];
      }
    }
    for(l=0; l<40; l++){
      light(all_block_k);
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

void sound(){
  tone(SP, 440);
  delay(10);
  noTone(SP);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("set1");
  Wire.begin();
  int i,j;
  mcp.begin_I2C();

  Serial.println("set2");
  //スピーカー
  pinMode(SP, OUTPUT);
  //ボタン
  pinMode(SW_A, INPUT_PULLUP);
  pinMode(SW_B, INPUT_PULLUP);
  attachInterrupt(SW_A, jump, FALLING);
  attachInterrupt(SW_B, go, FALLING);

  Serial.println("set3");
  //エキスパンダ（LED　カソード）
  for (i = 0; i < 2; i++) {
    Serial.println("set4");
    for(j = 0; j < 8; j++){
      if(pinN_mainus[i][j][1]){
        Serial.println("set5");
        mcp.pinMode(pinN_mainus[i][j][0], OUTPUT);
        Serial.println("set6");
        mcp.digitalWrite(pinN_mainus[i][j][0], HIGH);
        Serial.println("set7");
      }else{
        Serial.println("set8");
        pinMode(pinN_mainus[i][j][0], OUTPUT);
        Serial.println("set9");
        digitalWrite(pinN_mainus[i][j][0], HIGH);
      }
    }
    Serial.println("set10");
  }
  Serial.println("set11");
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
  /*
  while(1){
    GameStart();
    if(go_btn==1){
      break;
      stn_time = millis();
    }
  }*/
  Serial.println("set12");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("loop");
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
    gravity();
  }
}
