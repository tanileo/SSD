/*
 * 2022.08.30(Tue) 表示部分完成：片平
 * 2022.09.08(Thu) 加速度センサー変更版：今村
 * 2022.09.15(Thu) 時間を30sに調整、効果音を出す
 */
#include <Wire.h>
//#include "onkai.h"

#define MMA8452_ADRS 0x1D
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG  0x0E
#define CTRL_REG1 0x2A
#define CTRL_REG1_ACTV_BIT  0x01
#define G_SCALE 8     // range

#define pina 8
#define pinb 5
#define s 12
#define wa 100
#define it 641    // 0.04678 / 回
// #define debug 19

int xa[pina]={33, 26, 32, 25, 14, 15, 12, 13};
int xb[pinb]={27, 17, 2, 4, 16};
int mass[41]={0};
byte tmp;
byte buf[7];
unsigned long s_time = 0, e_time = 0;
float t;
bool st = 0;
bool Alarm_f = 0;

int angle(void){
  int16_t axRaw, ayRaw, azRaw;
  double roll;

  // MMA8452の内部レジスタにある測定値を読み込み
  ReadByteArray(0x00, 7, buf);
  
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

  if(roll > 0 && roll <= 180){
    // Serial.print("+\n");
    return 1;
  }else{
    // Serial.print("-\n");
    return 0;
  }    
}

void setup() {
  // put your setup code here, to run once:
  int i;
  for(i=0;i<pina;i++)
    pinMode(xa[i],OUTPUT);
  for(i=0;i<pinb;i++)
    pinMode(xb[i],OUTPUT);
  
  // pinMode(debug, OUTPUT);
  
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
}
/*void Alarm1()
{
  tone(23, So, 20);
  delay(20);
  tone(23, So2, 20);
  delay(100);
  tone(23, So3, 20);
  delay(20);  
}
void Alarm2()
{
  tone(23, Si2, 100);
  delay(100);
  tone(23, Mi3, 250);
  delay(250);  
}
void anoWrite(int a,int b)
{
  mass[a]=b;
}*/

void light(void)
{
  int i,j;
  for(j=0;j<pinb;j++)
    digitalWrite(xb[j],LOW);
  for(j=0;j<pina;j++)
    digitalWrite(xa[j],HIGH);
  for(i=0;i<8;i++){
    digitalWrite(xa[i],LOW);
    for(j=0;j<pinb;j++){
      if(mass[j+5*i+1])
      digitalWrite(xb[j],HIGH);
    }
    
    delayMicroseconds(wa);
    digitalWrite(xa[i],HIGH);
    for(j=0;j<pinb;j++){
      if(mass[j+5*i+1])
      digitalWrite(xb[j],LOW);
    }    
  }
}

void loop() {
  int p[4][12]={{18,12,14,13,9,7,8,1,5,2,4,3},{38,39,37,40,36,33,32,34,28,29,27,23},{23,29,27,28,32,34,33,40,36,39,37,38},{3,2,4,1,5,8,9,7,13,12,14,18}};
  int fall[2][3]={{23,28,33},{8,13,18}};
  int n[2][3][4]={{{18,13,8,3},{14,9,4,5},{12,7,2,1}},{{23,28,33,38},{27,32,37,36},{29,34,39,40}}};
  int m[3][2],r[3][2];
  int falls,f;
  int u,d;
  int i,j,k;
  int a=0,b=0,c=0;
  int g=0;

  if(angle()){  //UUU=Uが上]
    Serial.print("a");
    u=12;d=0;
    for(i=0;i<s;i++)
    anoWrite(p[0][i],1);
  }else{
    Serial.print("b");
    d=12;u=0;
    for(i=0;i<s;i++)
    anoWrite(p[2][i],1);
  }
  for(i=0; i<it; i++){
    // Serial.print("f");
    light();
    if(angle()!=g){
      g=(g+1)%2;
      i=0;
    }
  }
  
  Alarm1();
  while(1){
    if(st == 0 && (u == s || d == u)){
      s_time = millis();
      st = 1;
    }
    e_time = millis() - s_time;
    t = e_time / 1000.0;    
    Serial.println(t);
    
    if(angle()){
      Serial.print("Up\n");
      if(u == s && Alarm_f == 1){
        Serial.println("Start");
        Alarm1();
        st = 0;
        Alarm_f = 0;
      }
      if(d == s && Alarm_f == 0){
        Serial.println("End");
        Alarm2();
        Alarm_f = 1;
      }
      g=1;
      if(u)u--;
      anoWrite(p[0][u],0);
      anoWrite(fall[0][0],1);
      falls=0;
      f=0;
      r[0][0]=r[1][0]=r[2][0]=0;
      m[0][0]=((u>=7)?3:((u>=4)?2:((u>=1)?1:0)));
      m[1][0]=((u>=9)?3:((u>=5)?2:((u>=3)?1:0)));
      m[2][0]=((u>=8)?3:((u>=6)?2:((u>=2)?1:0)));
      m[0][1]=((d==0)?0:((d<6)?1:((d<9)?2:3)));
      m[1][1]=((d<3)?0:((d<5)?1:((d<7)?2:3)));
      m[2][1]=((d<2)?0:((d<4)?1:((d<8)?2:3)));
      r[0][1]=((d<6)?3:((d<9)?2:((d<12)?1:0)));
      r[1][1]=((d<7)?2:((d<11)?1:0));
      r[2][1]=((d<8)?2:((d<10)?1:0));    
    }else{
      Serial.print("Down\n");
      if(d == s && Alarm_f == 1){
        Serial.println("Start");
        Alarm1();
        st = 0;        
        Alarm_f = 0;
      }
      if(u == s && Alarm_f == 0){
        Serial.println("End");
        Alarm2();
        Alarm_f = 1;
      }
      g=0;
      if(d)d--;
      anoWrite(p[2][d],0);
      anoWrite(fall[1][2],1);
      falls=2;
      f=1;
      r[0][1]=r[1][1]=r[2][1]=0;
      m[0][1]=((d>=7)?3:((d>=4)?2:((d>=1)?1:0)));
      m[1][1]=((d>=9)?3:((d>=5)?2:((d>=3)?1:0)));
      m[2][1]=((d>=8)?3:((d>=6)?2:((d>=2)?1:0)));
      m[0][0]=((u==0)?0:((u<6)?1:((u<9)?2:3)));
      m[1][0]=((u<3)?0:((u<5)?1:((u<7)?2:3)));
      m[2][0]=((u<2)?0:((u<4)?1:((u<8)?2:3)));
      r[0][0]=((u<6)?3:((u<9)?2:((u<12)?1:0)));
      r[1][0]=((u<7)?2:((u<11)?1:0));
      r[2][0]=((u<8)?2:((u<10)?1:0));      
    }
    
    a=b=c=0;
    for(;!((a||u+d==s)&&b&&c);){
      // Serial.print("e");
      a=b=c=0;
      for(i=0; i<it; i++){
        // Serial.print("f");
        light();
        if(angle()!=g){
          g=(g+1)%2;
          i=0;
        }
      }
      if(angle()){
        g=1;
        k=1;
        if(!(u+d==s)){
          anoWrite(fall[f][falls],0);
          if((f==0&&falls+(d+1)/3>=3)||falls==2){
            a=1;
            // Serial.print("g");
          }else{
            falls++;
            // Serial.print("h");
          }          
          // Serial.print(falls);
        }
        if(!((m[0][0]==0&&((f==1&&a)||f==0))||r[0][0]<=1)){
          for(i=0;i<3;i++){
            if(m[i][0]){
              anoWrite(n[0][i][r[i][0] + m[i][0] - 1], 0);
              anoWrite(n[0][i][r[i][0] - 1], 1);
              r[i][0]--;
            }
            // Serial.print("move");
          }          
        }else{
          b=1;
          if(f==1&&!(u+d==s))
            u++;
          for(i=s-1;i>u-1;i--){
            anoWrite(p[0][i],0);
            // Serial.print("k");
          }
          for(i=0;i<u;i++){
            anoWrite(p[0][i],1);
            // Serial.print("l");
          }
          r[0][0]=r[1][0]=r[2][0]=0;
          m[0][0]=((u>=7)?3:((u>=4)?2:((u>=1)?1:0)));
          m[1][0]=((u>=9)?3:((u>=5)?2:((u>=3)?1:0)));
          m[2][0]=((u>=8)?3:((u>=6)?2:((u>=2)?1:0)));
          // Serial.print("moveFinish");
        }
        if(!(m[0][1]==0||r[1][1]+m[1][1]>=3||r[2][1]+m[2][1]>=3||(d<=3&&r[0][1]>=2))){
          for(i=0;i<3;i++){
            if(m[i][1]){
              anoWrite(n[1][i][r[i][1]],0);
              anoWrite(n[1][i][r[i][1]+m[i][1]],1);
              r[i][1]++;
              // Serial.print("n");
            }
          }
        }else{
          c=1;
          if(f==0&&a&&!(u+d==s))
            d++;
          for(i=s-1;i>d-1;i--){
            anoWrite(p[1][i],0);
            // Serial.print("o");
          }
          for(i=0;i<d;i++){
            anoWrite(p[1][i],1);
            // Serial.print("p");
          }
          m[0][1]=((d==0)?0:((d<6)?1:((d<9)?2:3)));
          m[1][1]=((d<3)?0:((d<5)?1:((d<7)?2:3)));
          m[2][1]=((d<2)?0:((d<4)?1:((d<8)?2:3)));
          r[0][1]=((d<6)?3:((d<9)?2:((d<12)?1:0)));
          r[1][1]=((d<7)?2:((d<11)?1:0));
          r[2][1]=((d<8)?2:((d<10)?1:0));
          // Serial.print("q");
        }
        if(!(u+d==s))
          anoWrite(fall[f][falls],1);
      }else{
        g=0;
        k=0;
        if(!(u+d==12)){
          anoWrite(fall[f][falls], 0);
          if((f==1&&-falls+(u+1)/3>=1)||falls==0){
            a=1;
            // Serial.print("r");
          }else{
            falls--;
            // Serial.print("s");
          }
          anoWrite(fall[f][falls],1);
          // Serial.print(falls);
        }
        if(!((m[0][1]==0&&((f==0&&a)||f==1))||r[0][1]<=1)){
          for(i=0;i<3;i++){
            if(m[i][1]){
              anoWrite(n[1][i][r[i][1]+m[i][1]-1], 0);
              anoWrite(n[1][i][r[i][1]-1], 1);
              r[i][1]--;
              // Serial.print("u");
            }
          }
          // Serial.print("\nb != 1\n");
          // Serial.print(m[0][0]); Serial.print(" ");  Serial.print(m[1][0]); Serial.print(" ");  Serial.print(m[2][0]); Serial.println(" ");
          // Serial.print(m[0][1]); Serial.print(" ");  Serial.print(m[1][1]); Serial.print(" ");  Serial.print(m[2][1]); Serial.println(" "); 
          // Serial.print(r[0][1]); Serial.print(" ");  Serial.print(r[1][1]); Serial.print(" ");  Serial.print(r[2][1]); Serial.println(" ");     
        }else{
          b=1;
          // Serial.print("\nb = 1\n");
          if(f==0&&!(u+d==s))d++;
          for(i=s-1;i>d-1;i--){
            anoWrite(p[2][i],0);
            // Serial.print("v");
          }
          for(i=0;i<d;i++){
            anoWrite(p[2][i],1);
            // Serial.print("w");
          }
          r[0][1]=r[1][1]=r[2][1]=0;
          m[0][1]=((d>=7)?3:((d>=4)?2:((d>=1)?1:0)));
          m[1][1]=((d>=9)?3:((d>=5)?2:((d>=3)?1:0)));
          m[2][1]=((d>=8)?3:((d>=6)?2:((d>=2)?1:0)));
          // Serial.print("x");
          // Serial.print(m[0][0]); Serial.print(" ");  Serial.print(m[1][0]); Serial.print(" ");  Serial.print(m[2][0]); Serial.println(" ");
          // Serial.print(m[0][1]); Serial.print(" ");  Serial.print(m[1][1]); Serial.print(" ");  Serial.print(m[2][1]); Serial.println(" "); 
          // Serial.print(r[0][1]); Serial.print(" ");  Serial.print(r[1][1]); Serial.print(" ");  Serial.print(r[2][1]); Serial.println(" ");     
        }
        if(!(m[0][0]==0||r[1][0]+m[1][0]>=3||r[2][0]+m[2][0]>=3||(u<=3&&r[0][0]>=2))){
          for(i=0;i<3;i++){
            if(m[i][0]){
              anoWrite(n[0][i][r[i][0]],0);
              anoWrite(n[0][i][r[i][0]+m[i][0]],1);
              r[i][0]++;
              // Serial.print("y");
            }
          }
        }else{
          c=1;
          if(f==1&&a&&!(u+d==s))u++;
          for(i=s-1;i>u-1;i--){
            anoWrite(p[3][i],0);
            // Serial.print("z");
          }
          for(i=0;i<u;i++){
            anoWrite(p[3][i],1);
            // Serial.print("1");
          }
          m[0][0]=((u==0)?0:((u<6)?1:((u<9)?2:3)));
          m[1][0]=((u<3)?0:((u<5)?1:((u<7)?2:3)));
          m[2][0]=((u<2)?0:((u<4)?1:((u<8)?2:3)));
          r[0][0]=((u<6)?3:((u<9)?2:((u<12)?1:0)));
          r[1][0]=((u<7)?2:((u<11)?1:0));
          r[2][0]=((u<8)?2:((u<10)?1:0));
          // Serial.print("2");
        }// Serial.print("3");
        if(!(u+d==s))
          anoWrite(fall[f][falls],1);
      }// Serial.print("4");
    }// Serial.print("5");
  }// Serial.print("6");
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
