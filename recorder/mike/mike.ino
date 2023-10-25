#define SPEAK 0
#define SPEAK_P A15
#define NUMbit 256
#define NUM 1000
#define INPUTpin 4
#define OUTPUTpin 5

int wave[NUM];
 int wave_cut[NUM];

void wave_to_cut(void){
  int i;
  for(i=0; i<NUM; i++){
    //wave[i] = wave[i]
  }
  
}
void setup() {
  Serial.begin(115200);
  ledcSetup(0,12800,8);
  ledcAttachPin(SPEAK_P,0);
  // put your setup code here, to run once:
  //pinMode(INPUTpin, INPUT);
  //pinMode(OUTPUTpin, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //int i;
  //for(i=0; i<NUM; i++){
    //wave[i] = analogRead(INPUTpin);
    int i = analogRead(INPUTpin);
    Serial.print(i);
    Serial.print("\n");
    delay(10);
  //}
  
  //for(i=0; i<NUM; i++){
    //ledcWrite(0,i);
    //delay(10);
  //}
}
