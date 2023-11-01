#define SPEAK 0
#define SPEAK_P A13
#define NUMbit 256
#define NUM 1000
#define INPUTpin 4
#define OUTPUTpin 5

int wave[NUM];

void setup() {
  Serial.begin(9600);
  ledcSetup(0,12800,13);
  ledcAttachPin(SPEAK_P,0);
  //pinMode(INPUTpin, INPUT);
  pinMode(OUTPUTpin, OUTPUT);
  
}

void loop() {

  int i;
  for(i=0; i<NUM; i++){
    wave[i] = analogRead(INPUTpin);
    Serial.print(wave[i]);
    Serial.print("\n");
    delay(1);
  }
  
  for(i=0; i<NUM; i++){
    ledcWrite(0,i);
    delay(1);
  }
}
