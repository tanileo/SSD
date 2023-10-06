#define TIME 2
#define ANALOG_NUM 200
#define ANALOG_NUM_MAX 200
#define DIGITAL_NUM 500
#define REP  5
#define PATTERN_PIN  15

int px[3]={12, 5, 13};
int py[2]={4, 16};

int pattern[][REP][2][3]={{{{1,1,1},{1,1,1}},{{1,0,0},{1,0,0}},{{0,1,0},{0,1,0}},{{0,0,1},{0,0,1}},{{0,0,0},{0,0,0}}},
                        {{{},{}},{{},{}},{{},{}},{{},{}},{{},{}}},
                        {{{},{}},{{},{}},{{},{}},{{},{}},{{},{}}},
                        {{{},{}},{{},{}},{{},{}},{{},{}},{{},{}}},
                        {{{},{}},{{},{}},{{},{}},{{},{}},{{},{}}}};
int i,j,k,l,m,n;
int current_pattern=0;

void position_level(){
  //Serial.print("position_level");
  for(i=0; i<REP; i++){
    for(j=0; j<ANALOG_NUM; j++){
      for(k=0; k<2; k++){
        for(l=0; l<2; l++){
          if(pattern[current_pattern][i][k][l]==1){
            analog_light_plus(px[l],px[k],j);
          }else{
            delay(TIME);
          }
        }
      }
    }
    for(j=0; j<DIGITAL_NUM; j++){
      for(k=0; k<2; k++){
        for(l=0; l<2; l++){
          if(pattern[current_pattern][i][k][l]==1){
            light(px[l],px[k]);
          }else{
            delay(TIME);
          }
        }
      }
    }
    for(j=0; j<ANALOG_NUM; j++){
      for(k=0; k<2; k++){
        for(l=0; l<2; l++){
          if(pattern[current_pattern][i][k][l]==1){
            analog_light_mainus(px[l],px[k],j);
          }else{
            delay(TIME);
          }
        }
      }
    }
  }
}

void light(int x, int y){
  //Serial.print(x);Serial.print(" ");Serial.print(ANALOG_NUM_MAX);Serial.print("\n");
  
  analogWrite(x, ANALOG_NUM_MAX);
  digitalWrite(y, LOW);
  delay(TIME);
  analogWrite(x, 0);
  digitalWrite(y, HIGH);
}

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

void setup() {
  // put your setup code here, to run once:
  for(i=0; i<2; i++){
    Serial.begin(115200);
    pinMode(py[i],OUTPUT);
    digitalWrite(py[i], HIGH);
  }
  analogWriteFreq(1000);
  analogWriteRange(ANALOG_NUM_MAX);  
}

void loop() {
  // put your main code here, to run repeatedly:
  position_level();
}
