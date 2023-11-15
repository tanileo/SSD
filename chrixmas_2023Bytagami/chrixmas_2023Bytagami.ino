// 2022.12.04(Sun) 谷　作
// 2022.12.07(Wed) 今村　改
// 2022.12.12(Mon) 谷　改改
// 2023.11.15(thu) 田上　改

#define PATTERN_MAX   2
#define REP 5             // patternの変化数(配列の3つ目の数)
#define ANALOG_NUM 400
#define ANALOG_NUM_MAX 800
#define DIGITAL_NUM 1000
#define TIME 1
#define MICRO_TIME 900
#define PATTERN  15
#define L1 25
#define L2 24
#define L3 23

// プロトタイプ宣言
void h_create(int n);
void position_level(int pattern[][2][3], int n);
void light(int x, int y);
void analog_light_plus(int x, int y, int henka);
void analog_light_minas(int x, int y, int henka);

// グローバル変数
int px[3]={L1, L2, L3};   // LED anode側：左からの並び
int py[2]={4, 16};       // LED cathode側：上からの並び
int pattern1[REP][2][3]={{{1,1,1},{1,1,1}}, {{1,0,1},{0,1,0}}, {{1,1,0},{1,0,1}}, {{1,0,1},{0,1,0}}, {{1,1,0},{1,0,1}}};
int pattern2[REP][2][3]={{{1,1,1},{1,1,1}}, {{1,0,0},{1,0,0}}, {{0,1,0},{0,1,0}}, {{0,0,1},{0,0,1}}, {{1,1,1},{1,1,1}}};
int create_h[REP][2][3];
int current_pattern = 1;
int flag=0;

int i, j, k, l, m, n;

void h_create(int n){
  switch(n){
    case 1:
      for(i=0; i<REP; i++){            // 光るパターンを代入し、position_level関数に渡す
        for(j=0; j<2; j++){
          for(k=0; k<3; k++){
            create_h[i][j][k] = pattern1[i][j][k];    // 光るパターン代入
          }
        }
      }
    break;
    case 2:
      for(i=0; i<REP; i++){            // 光るパターンを代入し、position_level関数に渡す
        for(j=0; j<2; j++){
          for(k=0; k<3; k++){
            create_h[i][j][k] = pattern2[i][j][k];    // 光るパターン代入
          }
        }
      }
    break;
  }
  position_level(create_h, REP);
}

void position_level(int pattern[][2][3], int n){       // create_hから渡された配列の1と0を判断して光らせるか光らせないか判断する光らす場合はlightのどれかに渡す

  for(i=0; i<REP; i++){                  //少しずつ光量をあげる
    if(flag){
      flag=0;
       break;
    }
    for(m=0; m<ANALOG_NUM*4; m++){
      if(flag){
        break;
      }
        for(k=0; k<2; k++){
          if(flag){
              break;
            }
          for(l=0; l<3; l++){
            if(flag){
              break;
            }
            if(pattern[i][k][l]==1){
              if(px[l]==5){
                m+=400;
              }else{
                m*=2;
              }
              //ESP.wdtFeed();
              analog_light_plus(px[l], py[k], m);
              if(px[l]==5){
                m-=400;
              }else{
                m/=2;
              }
            }else{
               // delay(TIME);
            }
          }
        
      }
    }
    
    for(n=0; n<DIGITAL_NUM; n++){     // 点滅
      if(flag){
        break;
      }
        for(k=0; k<2; k++){
          if(flag){
            break;
          }
          for(l=0; l<3; l++){
            if(flag){
              break;
            }
            if(pattern[i][k][l]==1){
             // ESP.wdtFeed();
              light(px[l], py[k]);
            }else{
              delay(TIME);
            }
          }
        }
      
    }
    
    for(m=0; m<ANALOG_NUM*3; m++){         // 少しずつ光量を下げる
     if(flag){
       break;
     }
        for(k=0; k<1; k++){
          if(flag){
            break;
          }
          for(l=0; l<3; l++){
            if(flag){
              break;
            }
            if(pattern[i][k][l]==1){
              if(px[l]==12||px[l]==13){
                m*=2;
              }
             // ESP.wdtFeed();
              analog_light_minas(px[l], py[k], m);
              if(px[l]==12||px[l]==13){
                m/=2;
              }
            }else{
               // delay(TIME);
            }
          }
        }
     
    }
  }
}

// On or Off
void light(int x, int y){
  int volt_set=0;
 if(px[l]==12||px[l]==13){
  volt_set=10;
 }
 Serial.print("light ");
 Serial.print(x);Serial.print(" ");
 Serial.print(ANALOG_NUM_MAX-volt_set);
 Serial.print("\n");
  analogWrite(x, ANALOG_NUM_MAX-volt_set);   // LED anode:Max
 // digitalWrite(y, LOW);         // LED cathode:Low
  delay(TIME);        
  //analogWrite(x, 0);            // LED anode:Min
  //digitalWrite(y, HIGH);        // LED cathode:Low
}
// 明るさが変化
void analog_light_plus(int x, int y, int henka){
  Serial.print("plus");
  Serial.print(x);Serial.print(" ");
  Serial.print( henka );
  Serial.print("\n");
  analogWrite(x, henka);      // LED anode:PWM
 // digitalWrite(y, LOW);       // LED cathode:Low
  delay(TIME);
  //analogWrite(x, 0);
  //digitalWrite(y, HIGH);
}

// 明るさが変化
void analog_light_minas(int x, int y, int henka){
  Serial.print("minas");
  Serial.print(x);Serial.print(" ");
  Serial.print( ANALOG_NUM_MAX-henka );
  Serial.print("\n");
  analogWrite(x, ANALOG_NUM_MAX - henka); // LED anode:PWM
  //digitalWrite(y,LOW);                // LED cathode:Low
  delay(TIME);
  //analogWrite(x,0);
  //digitalWrite(y,HIGH);
}


//void IRAM_ATTR btn1(void){
  //if(current_pattern==PATTERN_MAX){
  //  current_pattern=1;
  //}else{
   // current_pattern++;
  //}
  //flag=1;
//}

void setup() {
  Serial.begin(115200);
  // GPIO設定
  for(i=0; i<2; i++){
    pinMode(py[i], OUTPUT);
    digitalWrite(py[i], HIGH);
  }
  // PWN設定
  analogWriteFreq(1000);
  analogWriteRange(ANALOG_NUM_MAX);  

  // ボタンの設定：未使用の予定
  // pinMode(PATTERN, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(PATTERN), btn1, RISING);
}

void loop() {
  //ESP.wdtFeed();
  h_create(current_pattern);
}
