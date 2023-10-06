int i,j,p1[2]={4,16},p2[3]={5,12,13};

void setup() {
  // put your setup code here, to run once:
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(16,OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  

  Serial.print("a");
  
  for(i=0;i<2;i++)
  digitalWrite(p1[i],HIGH);
  for(i=0;i<3;i++)
  digitalWrite(p2[i],LOW);
  
  for(i=0;i<2;i++){
    for(j=0;j<3;j++){
      
      digitalWrite(p1[i],LOW);
      digitalWrite(p2[j],HIGH);
      Serial.print(p1[i]);
      Serial.print(p2[j]);
      delay(1000);
      digitalWrite(p1[i],HIGH);
      digitalWrite(p2[j],LOW);
   }
  }
}
