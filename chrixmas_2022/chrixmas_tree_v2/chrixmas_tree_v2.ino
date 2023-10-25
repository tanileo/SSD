// 2022.12.04(Sun) 谷　作
// 2022.12.07(Wed) 今村　改
// ・ボタンは使用しないかも

#define MAX_VOLT 3.3
#define PATTERN_MAX   5
//#define VOLUM_MAX     5
#define REP 5             // patternの変化数(配列の1つ目の数)
#define ANALOG_NUM 200
#define DIGITAL_NUM 1000
//#define K 0.001            //analogWriteの傾き係数
#define TIME 1
#define PATTERN  9
// #define VOLUMp  10
// #define VOLUMm  11

// プロトタイプ宣言
void h_create(int n);
void position_level(int pattern[][2][3], int n);
void light(int x, int y);
void analog_light_plus(int x, int y, int henka);
void analog_light_minas(int x, int y, int henka);

// グローバル変数
int px[3]={12, 5, 13};   // LED anode側：左からの並び
int py[2]={4, 16};       // LED cathode側：上からの並び
int pattern1[REP][2][3]={{{1,1,1},{1,1,1}}, {{1,0,1},{0,1,0}}, {{1,1,0},{1,0,1}}, {{1,0,1},{0,1,0}}, {{1,1,0},{1,0,1}}};
int create_h[REP][2][3];
int current_pattern = 1;
int current_volum = 1;
int emergency_exit;

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
            create_h[i][j][k] = pattern1[i][j][k];    // 光るパターン代入
          }
        }
      }
    break;
  }
  position_level(create_h, REP);
}

void position_level(int pattern[][2][3], int n){       // create_hから渡された配列の1と0を判断して光らせるか光らせないか判断する光らす場合はlightのどれかに渡す
  for(i=0; i<REP; i++){                  //少しずつ光量をあげる
    for(m=0; m<ANALOG_NUM; m++){
      for(j=0; j< REP; j++){
        for(k=0; k<2; k++){
          for(l=0; l<3; l++){
            //if(emergency_exit){
              //break;
            //}
            if(pattern[j][k][l]==1){
              analog_light_plus(px[l], py[k], m);
            }else{
               //delay(TIME);
            }
          }
        }
      }
    }
    
    for(n=0; n<DIGITAL_NUM; n++){     // 点滅
      for(j=0; j< REP; j++){
        for(k=0; k<2; k++){
          for(l=0; l<3; l++){
            for(l=0; l<3; l++){
              //if(emergency_exit){
                //emergency_exit=0;
                //break;
              //}
              if(pattern[j][k][l]==1){
                light(px[l], py[k]);
              }else{
                delay(TIME);
              }
            }
          }
        }
      }
    }
    for(m=0; m<ANALOG_NUM; m++){         // 少しずつ光量を下げる
      for(j=0; j< REP; j++){
        for(k=0; k<2; k++){
          for(l=0; l<3; l++){
            for(l=0; l<3; l++){
              //if(emergency_exit){
                //emergency_exit=0;
                //break;
              //}
              if(pattern[j][k][l]==1){
                analog_light_minas(px[l], py[k], m);
              }else{
                //delay(TIME);
              }
            }
          }
        }
      }
    }
  }
}

// On or Off
void light(int x, int y){
  analogWrite(x, ANALOG_NUM);   // LED anode:Max
  digitalWrite(y, LOW);         // LED cathode:Low
  delay(TIME);        
  analogWrite(x, 0);            // LED anode:Min
  digitalWrite(y, HIGH);        // LED cathode:Low
}
// 明るさが変化
void analog_light_plus(int x, int y, int henka){
  analogWrite(x, henka);      // LED anode:PWM
  digitalWrite(y, LOW);       // LED cathode:Low
  // delay(TIME);
  // analogWrite(x, 0);
  // digitalWrite(y, HIGH);
}

// 明るさが変化
void analog_light_minas(int x, int y, int henka){
  analogWrite(x, ANALOG_NUM-henka); // LED anode:PWM
  digitalWrite(y,LOW);                // LED cathode:Low
  // delay(TIME);
  // digitalWrite(x,LOW);
  // digitalWrite(y,HIGH);
}


void btn1(void){
  if(current_pattern==PATTERN_MAX){
    current_pattern=1;
  }else{
    current_pattern++;
  }
  emergency_exit=1;
}
/*
void btn2(void){
  if(current_volum!=VOLUM_MAX){
    current_volum++;
  }
}

void btn3(void){
  if(current_volum!=0){
    current_volum--;
  }
}
*/

void setup() {
  //for(i=0; i<3; i++){
    //pinMode(px[i], OUTPUT);
  //}
  
  // GPIO設定
  for(i=0; i<2; i++){
    pinMode(py[i], OUTPUT);
    digitalWrite(py[i], HIGH);
  }
  // PWN設定
  analogWriteFreq(1000);
  //analogWriteSetDefaultFreq(1000);
  analogWriteRange(200);  

  // ボタンの設定：未使用の予定
  pinMode(PATTERN, INPUT_PULLUP);
  attachInterrupt(PATTERN, btn1, HIGH);
  // pinMode(VOLUMp, INPUT_PULLUP);
  // attachInterrupt(VOLUMp, btn2, LOW);
  // pinMode(VOLUMm, INPUT_PULLUP);
  // attachInterrupt(VOLUMm, btn3, LOW);
}


void loop() {
  h_create(current_pattern);
}
