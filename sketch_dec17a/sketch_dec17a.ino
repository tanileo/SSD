#define PATTERN_MAX   2
#define REP 5             // patternの変化数(配列の3つ目の数)
#define ANALOG_NUM 400
#define ANALOG_NUM_MAX 800
#define DIGITAL_NUM 400
#define TIME 2
#define MICRO_TIME 
#define PATTERN  15

//プロトタイプ宣言
void h_create(int n);
void position_level(int pattern[][2][3], int n);
void light(int x, int y);
void analog_light_plus(int x, int y, int henka);
void analog_light_minas(int x, int y, int henka);

//グローバル変数
int px[3]={12, 5, 13};
int py[2]={4, 16};
int pattern1[REP][2][3]={{{1,1,1},{1,1,1}}, {{1,0,1},{0,1,0}}, {{1,1,0},{1,0,1}}, {{1,0,1},{0,1,0}}, {{1,1,0},{1,0,1}}};
int pattern2[REP][2][3]={{{1,1,1},{1,1,1}}, {{1,0,0},{1,0,0}}, {{0,1,0},{0,1,0}}, {{0,0,1},{0,0,1}}, {{1,1,1},{1,1,1}}};
int create_h[REP][2][3];
int current_pattern = 1;
int flag=0;
int wari_a=0;
int wari_b=0;

int i, j, k, l, m, n;

void h_create(int z){
  switch(z){
    case 1: 
        for(i=0; i<REP; i++){
          for(j=0; j<2; j++){
            for(k=0; k<3; k++){
              create_h[i][j][k] = pattern1[i][j][k];
            }
          }
        }break;
    
  }
  position_level(create_h,REP);
}

void position_level(int pattern[][2][3], int y){

  for(i=0; i<REP; i++){
    for(l=0; l<ANALOG_NUM; l++){
      for(j=0; j<2; j++){
        for(k=0; k<3; k++){
          if(pattern[i][j][k]==1){
            analog_light_plus(px[k],py[j], l);
          }else{
            delay(TIME);
          }
        }
      }
    }
    for(m=0; m<DIGITAL_NUM; m++){
      for(j=0; j<2; j++){
        for(k=0; k<3; k++){
          if(pattern[i][j][k]==1){
            light(px[k],py[j]);
          }else{
            delay(TIME);
          }
        }
      }
    }
    for(n=0; n<ANALOG_NUM; n++){
      for(j=0; j<2; j++){
        for(k=0; k<3; k++){
          if(pattern[i][j][k]==1){
            analog_light_mainus(px[k],py[j], n);
          }else{
            delay(TIME);
          }
        }
      }
    }
  }
}

void light(int x,int y){
  ESP.wdtFeed();
 Serial.print("light ");
 Serial.print(x);Serial.print(" ");
 Serial.print(ANALOG_NUM_MAX);
 Serial.print("\n");
  analogWrite(x,ANALOG_NUM_MAX);
  digitalWrite(y, LOW);
  delay(TIME);
  analogWrite(x, 0);
  digitalWrite(y, HIGH);
}

void analog_light_plus(int x, int y, int henka){
  ESP.wdtFeed();
  Serial.print("minas");
  Serial.print(x);Serial.print(" ");
  Serial.print( ANALOG_NUM_MAX-henka );
  Serial.print("\n");
  analogWrite(x, henka);
  digitalWrite(y, LOW);
  delay(TIME);
  analogWrite(x, 0);
  digitalWrite(y, HIGH);
}

void analog_light_mainus(int x, int y, int henka){
  ESP.wdtFeed();
  Serial.print("minas");
  Serial.print(x);Serial.print(" ");
  Serial.print( ANALOG_NUM_MAX-henka );
  Serial.print("\n");
  Serial.print("minas");
  Serial.print(x);Serial.print(" ");
  Serial.print( ANALOG_NUM_MAX-henka );
  Serial.print("\n");
  analogWrite(x, ANALOG_NUM_MAX - henka); // LED anode:PWM
  digitalWrite(y,LOW);                // LED cathode:Low
  delay(TIME);
  analogWrite(x,0);
  digitalWrite(y,HIGH);
}

void setup() {
  // put your setup code here, to run once:
  for(i=0; i<2; i++){
    pinMode(py[i], OUTPUT);
    digitalWrite(py[i], HIGH);
  }
  // PWN設定
  analogWriteFreq(1000);
  analogWriteRange(ANALOG_NUM_MAX);  

}

void loop() {
  // put your main code here, to run repeatedly:
  ESP.wdtFeed();
  h_create(current_pattern);
}
