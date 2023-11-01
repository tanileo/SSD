#define TIME 2
//#define ANALOG_NUM 200
//#define ANALOG_NUM_MAX 200
#define DIGITAL_NUM 100
#define REP  5
#define PATTERN_MAX 4
#define PATTERN_PIN 15

int px[3]={12, 5, 13};
int py[2]={4, 16};

int pattern[5][REP][2][3]={{{{1,1,1},{1,1,1}},{{1,0,0},{1,0,0}},{{0,1,0},{0,1,0}},{{0,0,1},{0,0,1}},{{0,0,0},{0,0,0}}},
                           {{{0,1,0},{0,0,0}},{{1,1,1},{0,0,0}},{{1,1,1},{1,0,1}},{{1,1,1},{1,1,1}},{{0,0,0},{0,0,0}}},
                           {{{0,0,0},{1,0,1}},{{0,0,0},{1,1,1}},{{1,0,1},{1,1,1}},{{1,1,1},{1,1,1}},{{0,0,0},{0,0,0}}},
                           {{{1,0,0},{0,1,1}},{{0,0,1},{1,1,0}},{{1,1,1},{0,0,0}},{{0,0,0},{1,1,1}},{{1,1,1},{1,1,1}}},
                           {{{},{}},{{},{}},{{},{}},{{},{}},{{},{}}}};
int i,j,k,l,m,n;
int current_pattern=0;
int wari_a=0,wari_b=0;
int flag=0;

void position_level();
void light(int x, int y);
//void analog_light_plus(int x, int y, int henka);
//void analog_light_mainus(int x, int y, int henka);
void btn();

void position_level(){
  //Serial.print("position_level");
  for(i=0; i<REP; i++){
    if(flag){
        flag=0;
        break;
    }
/*
    for(j=0; j<ANALOG_NUM; j++){
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
          wari_a+=digitalRead(PATTERN_PIN);
          if(wari_a){
            if(wari_a==wari_b){
              //Serial.print("wari_b");
              wari_a=wari_b=0;
              btn();
            }else{
              wari_b=wari_a;
              //Serial.print("wari_a");
            }
          }
          if(pattern[current_pattern][i][k][l]==1){
            analog_light_plus(px[l],py[k],j);
          }else{
            delay(TIME);
          }
        }
      }
*/
    for(j=0; j<DIGITAL_NUM; j++){
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
          wari_a+=digitalRead(PATTERN_PIN);
          if(wari_a){
            if(wari_a==wari_b){
              //Serial.print("wari_b");
              wari_a=wari_b=0;
              btn();
            }else{
              wari_b=wari_a;
              //Serial.print("wari_a");
            }
          }
          if(pattern[current_pattern][i][k][l]==1){
            light(px[l],py[k]);
          }else{
            delay(TIME);
          }
        }
      }
    }
/*
    for(j=0; j<ANALOG_NUM; j++){
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
          wari_a+=digitalRead(PATTERN_PIN);
          if(wari_a){
            if(wari_a==wari_b){
              //Serial.print("wari_b");
              wari_a=wari_b=0;
              btn();
            }else{
              wari_b=wari_a;
              //Serial.print("wari_a");
            }
          }
          if(pattern[current_pattern][i][k][l]==1){
            analog_light_mainus(px[l],py[k],j);
          }else{
            delay(TIME);
          }
        }
      }
    }
  }*/
}
}
void light(int x, int y){
  ESP.wdtFeed();
  //Serial.print(x);Serial.print(" ");Serial.print(ANALOG_NUM_MAX);Serial.print("\n");
  //Serial.print("light");Serial.print(" ");Serial.print("x");Serial.print(x);Serial.print("");Serial.print("y");Serial.print(y);
  digitalWrite(x, HIGH);
  digitalWrite(y, LOW);
  delay(TIME);
  digitalWrite(x, 0);
  digitalWrite(y, HIGH);
}
/*
void analog_light_plus(int x, int y, int henka){
  //Serial.print(x);Serial.print(" ");Serial.print(henka);Serial.print("\n");
  analogWrite(x, henka);
  digitalWrite(y, LOW);
  delay(TIME);
  analogWrite(x, 0);
  digitalWrite(y, HIGH);
}

void analog_light_mainus(int x, int y, int henka){
  //Serial.print(x);Serial.print(" ");Serial.print(ANALOG_NUM_MAX-henka);Serial.print("\n");
  analogWrite(x, ANALOG_NUM_MAX-henka);
  digitalWrite(y, LOW);
  delay(TIME);
  analogWrite(x, 0);
  digitalWrite(y, HIGH);
}
*/

void btn(){
  if(current_pattern==(PATTERN_MAX-1)){
    current_pattern=0;
  }else{
    current_pattern++;
  }
  Serial.print(current_pattern);
  flag=1;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for(i=0; i<2; i++){
    pinMode(py[i],OUTPUT);
    digitalWrite(py[i], HIGH);
  }
  for(i=0; i<3; i++){
    pinMode(px[i],OUTPUT);
    digitalWrite(px[i], LOW);
  }
  pinMode(PATTERN_PIN, INPUT);
  
  //analogWriteFreq(1000);
  //analogWriteRange(ANALOG_NUM_MAX);  
}

void loop() {
  // put your main code here, to run repeatedly:
  position_level();
}
