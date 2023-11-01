#define SPEAK 0
#define SPEAK_P A15
#define NUM 256
//int music[NUM]={};
int h;

void heltz(int x,int *h){
  *h=1000000/x/2;
}

void setup() {
  // 左から channel, 周波数, 解像度(8bit=256段階)
  ledcSetup(0,12800,8);
  ledcAttachPin(SPEAK_P,0);
  Serial.begin(12800);
}

void loop() {
  heltz(220, &h);
  int i=0;
  for(i=0; i<NUM; i++){
    ledcWrite(0,i);
    delay(h);
    ledcWrite(0,0);
    delay(h);
  }
  Serial.print('end');
}
