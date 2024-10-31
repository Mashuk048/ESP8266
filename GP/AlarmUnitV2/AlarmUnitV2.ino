#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();


#define alarmPin 0 //D3
#define dataIn 2 //D4  ONBOARD LED
#define ledPin 14//D5
#define sosButton 12//D6
//const byte sosButton = 12;//D6
volatile byte interruptCounter = 0;

int msgCounter=0;



// Holds the current button state.
volatile int state;
bool isPRESSED = false;

// Holds the last time debounce was evaluated (in millis).
volatile long lastDebounceTime = 0;

// The delay threshold for debounce checking.
const int debounceDelay = 30;

// pin is 2.
const int   buttonInput = 12;

//for long press detection
unsigned long counter = 0;



// Gets called by the interrupt.
void   ICACHE_RAM_ATTR   ISR() {
  // Get the pin reading.
  int reading = digitalRead(buttonInput);

  // Ignore dupe readings.
  if(reading == state) return;

  boolean debounce = false;
  
  // Check to see if the change is within a debounce delay threshold.
  if((millis() - lastDebounceTime) <= debounceDelay) {
    debounce = true;
  }

  // This update to the last debounce check is necessary regardless of debounce state.
  lastDebounceTime = millis();

  // Ignore reads within a debounce delay threshold.
  if(debounce) return;

  // All is good, persist the reading as the state.
  state = reading;
  isPRESSED = state;// if TRUE(HIGH),  then start counting after resetting counter
  counter = 0;

  if(state){//if button pin is read high
//    digitalWrite(alarmPin, HIGH);
    setAlarmON();
  }
  
//  else{counter=0;}//reset the counter when the button is released, it is dangerous cause if button press release is missed, it will never reset, overflow happens, wdt reset happens
//so reset the counter somewhere else where it is guaranteed such as in ISR
  
  // Work with the value now.
//  Serial.println("button: " + String(reading));
  
}//ISR ENDS




void setup() {
  pinMode(alarmPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(sosButton, INPUT);
//  pinMode(sosButton, INPUT_PULLUP);
//  attachInterrupt(digitalPinToInterrupt(sosButton), ISR, CHANGE);
  attachInterrupt(buttonInput, ISR,  CHANGE);
  Serial.begin(115200);
  Serial.println("RX READY");
  delay(500);
  mySwitch.enableReceive(dataIn);  // Receiver on interrupt 0 => that is pin #2 
}

void loop() {


    while(isPRESSED){    
          
      counter++;
      
      Serial.println(counter);
      if(counter > 3000){
        yield();
        setAlarmOFF();        
        counter = 0;
      }
    }





  
  if (mySwitch.available()) {
    
    Serial.print("Received ");

    unsigned long data = mySwitch.getReceivedValue();
    
    Serial.print( data );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );



    msgCounter++;
    Serial.println(msgCounter);
    if(msgCounter >= 4){
        if(data == 5555){
          Serial.println("Matched");
          setAlarmON();
        }
        else{
          Serial.println("Didn't match");
        }

        msgCounter = 0;
    }
    mySwitch.resetAvailable();    
  }  
    
}//LOOP ENDS



void setAlarmON(){
  Serial.println("Alarming...");
  digitalWrite(alarmPin, HIGH);
  digitalWrite(ledPin, HIGH);
}

void setAlarmOFF(){
  Serial.println("Alarm OFF");
  digitalWrite(alarmPin, LOW);
  digitalWrite(ledPin, LOW);
}