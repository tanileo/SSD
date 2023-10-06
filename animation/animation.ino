#include <Adafruit_MCP23X17.h>

#define SP 27
#define SW_A 14
#define SW_B 12
#define TOGGLE_B 19
#define ANI_NUM 10
#include <Wire.h>

int pinN_mainus[2][8][2]={{{9, 1}, {12, 1}, {15, 1}, {0, 0}, {6, 1}, {3, 1}, {0, 1}, {33, 0}}, {{8, 1}, {11, 1}, {14, 1}, {2, 0}, {7, 1}, {4, 1}, {1, 1}, {25, 0}}};
int pinN_plus[8][2]={{10, 1}, {13, 1}, {15, 0}, {4, 0}, {5, 1}, {2, 1}, {26, 0}, {32, 0}};

 Adafruit_MCP23X17 mcp;

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
  GameStart();
}
