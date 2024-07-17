/*
 * 2022.09.15(Thu) 谷作
 * GPIO21：SDA　
 * GPIO22：SCL
 */

#include <Wire.h>
#define MMA8452_ADRS 0x1D
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG  0x0E
#define CTRL_REG1 0x2A
#define CTRL_REG1_ACTV_BIT  0x01
#define G_SCALE 8     // range

byte tmp;
byte buf[7];
int pinN[9]={23, 19, 32, 33, 25, 26, 27, 14, 12};   // 配列の0番目にプラス側（プラスピン）　、配列の1~8番目にマイナス側（マイナスピン）のマトリックスのピンをつける
int num = 1;        // 光らせたい所の番号

int angle(void){
  int16_t axRaw, ayRaw, azRaw;
  double roll, pitch;
  
  // MMA8452の内部レジスタにある測定値を読み込み
  ReadByteArray(0x00, 7, buf);
  // Serial.print("Status:"); Serial.println(buf[0], HEX);
  // Serial.print(buf[1], HEX); Serial.print(" "); Serial.print(buf[2], HEX); Serial.print("\t");
  // Serial.print(buf[3], HEX); Serial.print(" "); Serial.print(buf[4], HEX); Serial.print("\t");
  // Serial.print(buf[5], HEX); Serial.print(" "); Serial.print(buf[6], HEX); Serial.print("\t");
  
  axRaw= (buf[1] << 8 | buf[2]) >> 4;
  ayRaw= (buf[3] << 8 | buf[4]) >> 4;
  azRaw= (buf[5] << 8 | buf[6]) >> 4;

  if(axRaw > 2047)
    axRaw -= 4096;
  if(ayRaw > 2047)
    ayRaw -= 4096;
  if(azRaw > 2047)
    azRaw -= 4096;

  roll  = atan2(ayRaw, azRaw) * RAD_TO_DEG;
  // pitch = atan(-axRaw / sqrt(ayRaw * ayRaw + azRaw * azRaw)) * RAD_TO_DEG;
  //Serial.print(roll);  Serial.print(",");
  //Serial.print(pitch);  Serial.println("");

  if(roll > 45 && roll <= 180){
    Serial.print("+\n");
    return 1;
  }else if(roll < -45 && roll >= -180){
    Serial.print("-\n");
    return -1;
  }else{
    Serial.print("=\n");
    return 0;
  }
    
}
void low(int x){
  Serial.println("low");
  for(int i=1; i<9; i++){
    if(i!=x){                       // 光らせたい場所以外のマイナスピンをHIGHにする
      digitalWrite(pinN[i], HIGH);
    }else{
      digitalWrite(pinN[i], LOW);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.setClock(400000);
  Wire.begin();

  //加速度センサー
  // MMA8452の内部レジスタを設定
  tmp = ReadByte(CTRL_REG1);
  WriteByte(CTRL_REG1, tmp &~(CTRL_REG1_ACTV_BIT)); // stanby mode
  WriteByte(XYZ_DATA_CFG,(G_SCALE >> 2) ); //range ±8
  tmp = ReadByte(CTRL_REG1);
  WriteByte(CTRL_REG1, tmp | CTRL_REG1_ACTV_BIT | 0x00);   // active mode 他
  tmp = ReadByte(0x0D); // who am i
  Serial.println(tmp, HEX); // 0x2A
  
  for(int i=0; i< 9; i++){
    pinMode(pinN[i], OUTPUT);
    digitalWrite(pinN[i], HIGH); //pinの設定    
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  low(num);                       // 光るべきもの以外は、消灯
  delay(1000);                    // 要らないのでは？
  num+=angle();
  if(angle()==1 && num > 8){
    num=1;
  }else if(angle()==-1 && num < 1){
    num=8;
    
  }
}
void ReadByteArray(byte adrs, int datlen, byte *dest)
{
  int i;
  Wire.beginTransmission(MMA8452_ADRS);
  Wire.write(adrs);
  Wire.endTransmission(false);
  Wire.requestFrom(MMA8452_ADRS, datlen);
  while(Wire.available() < datlen);
  for(int i = 0 ; i < datlen ; i++){
    dest[i] = Wire.read();
  }
}

byte ReadByte(byte adrs)
{
  Wire.beginTransmission(MMA8452_ADRS);
  Wire.write(adrs);
  Wire.endTransmission(false);
  Wire.requestFrom(MMA8452_ADRS, 1);
  while(!Wire.available());
  return(Wire.read());
}

void WriteByte(byte adrs, byte dat)
{
  Wire.beginTransmission(MMA8452_ADRS);
  Wire.write(adrs);
  Wire.write(dat);
  Wire.endTransmission();
}
