


const int sensorPin = D1;//D5
unsigned int rpm;



// Holds the current button state.
volatile int state = 1;

volatile byte interruptCounter = 0;

// Holds the last time debounce was evaluated (in millis).
volatile long lastDebounceTime = 0;

// The delay threshold for debounce checking.
const int debounceDelay = 30;

//snooze time
unsigned long interval = 60000; //in ms.  5 minutes snooze time default


//TIMER FOR RPM
unsigned long lastRpmCalDoneTime = 0;



// Gets called by the interrupt.
void   ICACHE_RAM_ATTR   ISR() {
  // Get the pin reading.
  int reading = digitalRead(sensorPin);

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


  if(!state){//if button pin is read 1(High Pulse) or read 0(LOW Pulse), change accordingly and remember it is a guaranteed press 
      interruptCounter++;
  }
  
}//ISR ENDS




void setup() {    
    
    pinMode(sensorPin, INPUT);
  
    attachInterrupt(sensorPin, ISR,  CHANGE);

}


void loop() {

    setRPMTimer();
    
}//LOOP ENDS


void setRPMTimer(){
  unsigned long now = millis();
  if(now - lastRpmCalDoneTime > 60000){
    lastRpmCalDoneTime = now;
    //write code here
    rpm = interruptCounter;
    Serial.println(rpm);    
    interruptCounter = 0;    
  }  
}
