#define SWA 14
#define SWB 12

#include <Adafruit_MCP23X17.h>
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

int current_btn = 0;
int pinN_mainus[2][8][2]={{{9, 1}, {12, 1}, {15, 1}, {0, 0}, {6, 1}, {3, 1}, {0, 1}, {33, 0}}, {{8, 1}, {11, 1}, {14, 1}, {2, 0}, {7, 1}, {4, 1}, {1, 1}, {25, 0}}};
int pinN_plus[8][2]={{10, 1}, {13, 1}, {15, 0}, {4, 0}, {5, 1}, {2, 1}, {26, 0}, {32, 0}};
int user_position = 6;
int user_position = 0;
int past_user_position = 0;
int current_block[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int all_block[5][8]={{0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0},
                    {0,0,1,0,0,0,0,0},
                    {0,0,1,0,0,0,0,0},
                    {2,2,2,2,2,2,2,2}};

  
 Adafruit_MCP23X17 mcp;




void light(void){
  int i,j;
  for(i=0; i<8; i++){
    for(j=0; j<8; j++){
      Serial.println(i, j);
         mcp.digitalWrite(pinN_mainus[0][j],LOW); digitalWrite(pinN_plus[i],HIGH);
        //mcp.digitalWrite(pinN_mainus[1][j],LOW); digitalWrite(pinN_plus[i],HIGH);
        delay(1);
      
        mcp.digitalWrite(pinN_mainus[0][j],HIGH); digitalWrite(pinN_plus[i],LOW);
        //mcp.digitalWrite(pinN_mainus[1][j],HIGH); digitalWrite(pinN_plus[i],LOW);
        delay(1);
    }
  }
  
}

void light2(void){
  mcp.digitalWrite(pinN_mainus[1][1],LOW); digitalWrite(pinN_plus[0],HIGH);
  delay(100);
  mcp.digitalWrite(pinN_mainus[1][1],HIGH); digitalWrite(pinN_plus[0],LOW);
  delay(10);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  int i,j;
  mcp.begin_I2C();
  for (i = 0; i < 2; i++) {  // 8~15(GPB0~GPB7)を出力に設定
    for(j = 0; j < 8; j++){
      mcp.pinMode(pinN_mainus[i][j], OUTPUT);
      mcp.digitalWrite(pinN_mainus[i][j], HIGH);
    }
  }
  
  
  
  for(i=0; i<8; i++){
    pinMode(pinN_plus[i], OUTPUT);
    digitalWrite(pinN_plus[i], LOW);
  }
    
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("loop");
  light();

}
