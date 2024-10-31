
const int led = D1;


void setup() {
  
  pinMode(D1, OUTPUT);
    
  analogWriteFreq(500); //Set PWM frequency 500, default is 1000
  analogWriteRange(255);  //Set range 0~255, default is 0~1023
}

// the loop function runs over and over again forever
void loop() {

  int i;
    
  for(i=0; i<=255; i++){
    analogWrite(led, i);
    delay(10);
  }
  //analogWrite(led, 0);

  
  for(i=255; i>=0; i--){
    
    analogWrite(led, i);
    
    delay(10);
  }

}
