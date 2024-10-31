const byte interruptPin = 12;
volatile byte interruptCounter = 0;
int numberOfInterrupts = 0;
int buttonState = 0;

#define ledPin 0



void ICACHE_RAM_ATTR revDetectorISR() {
  // ISR code
  interruptCounter++;
  buttonState = digitalRead(interruptPin);
  digitalWrite(ledPin, !buttonState);
}

void setup() {
 
  Serial.begin(115200);
  Serial.println("Ready");
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), revDetectorISR, RISING);
 
}
 
//void handleInterrupt() {
//  interruptCounter++;
//}



 
void loop() {
 
  if(interruptCounter>0){
 
      interruptCounter--;
      numberOfInterrupts++;
 
      Serial.print("An interrupt has occurred. Total: ");
      Serial.println(numberOfInterrupts);//numberOfInterrupts
  }
}
