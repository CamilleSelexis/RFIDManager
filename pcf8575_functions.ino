void init_pcf8575(int adr){
  PCF8575 pcf8575(adr); //Define the pcf8575 with its adr
  pcf8575.begin();
  /*
  pcf8575.pinMode(P0,INPUT);
  pcf8575.pinMode(P1,INPUT);
  pcf8575.pinMode(P2,INPUT);
  pcf8575.pinMode(P3,INPUT);
  pcf8575.pinMode(P4,INPUT);
  pcf8575.pinMode(P5,INPUT);
  pcf8575.pinMode(P6,INPUT);
  pcf8575.pinMode(P7,INPUT);
  */
  
  pcf8575.pinMode(P0,OUTPUT);
  pcf8575.pinMode(P1,OUTPUT);
  pcf8575.pinMode(P2,OUTPUT);
  pcf8575.pinMode(P3,OUTPUT);
  pcf8575.pinMode(P4,OUTPUT);
  pcf8575.pinMode(P5,OUTPUT);
  pcf8575.pinMode(P6,OUTPUT);
  pcf8575.pinMode(P7,OUTPUT);
  

  pcf8575.digitalWrite(P0,LOW);
  pcf8575.digitalWrite(P1,LOW);
  pcf8575.digitalWrite(P2,LOW);
  pcf8575.digitalWrite(P3,LOW);
  pcf8575.digitalWrite(P4,LOW);
  pcf8575.digitalWrite(P5,LOW);
  pcf8575.digitalWrite(P6,LOW);
  pcf8575.digitalWrite(P7,LOW);

}
//Enable the magnet on the given row - row 0 is on the left
bool enable_SSR(byte row){
  pcf8575.pinMode(row,OUTPUT);
  pcf8575.digitalWrite(row,HIGH);
}

//rows is 0 or 4
void disable_SSR(byte row){
  //Stop the timer and reset the bool varaible
  pcf8575.pinMode(row,OUTPUT);
  pcf8575.digitalWrite(row,LOW);
}
