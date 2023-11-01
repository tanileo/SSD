int Do = 261;
int Re = 294;
int Mi = 329;
int Fa = 349;
int So = 392;
int La = 440;
int Si = 493;
int Do2 = 523;
int Re2 = 587;
int Re2s = 622;
int Mi2 = 659;
int Fa2 = 698;
int Fa2s = 740;
int So2 = 784;
int La2 = 880;
int Si2 = 988;
int Do3 = 1047;
int Re3 = 1175;
int Mi3 = 1319;
int Fa3 = 1397;
int So3 = 1568;
 
int qd = 6; //Quarter Dotted note
int q =  4; //Quarter note
int ed = 3; //Eighth Dotted note
int e = 2; //Eighth note
int s = 1; //Sixteenth note
 
int harry_melody[14] = {Si, Mi2, So2, Fa2s, Mi2, Si2, La2, Fa2s, Mi2, So2, Fa2s, Re2s, Fa2, Si};
int harry_rhythm[14]  = {e, ed,   s,   e,    q,    e,   qd,  qd,  ed,   s,   e,   q,   e,   qd + q};
 
void setup() {

}
 
void loop() {
 
    for (int i = 0; i < 14; i++) {
      tone(12, harry_melody[i], harry_rhythm[i] * 200);
      delay(harry_rhythm[i] * 200);
    }
}
