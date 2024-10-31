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



#define alarmPin 0 //D3






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
    digitalWrite(alarmPin, HIGH);
  }
  
//  else{counter=0;}//reset the counter when the button is released, it is dangerous cause if button press release is missed, it will never reset, overflow happens, wdt reset happens
//so reset the counter somewhere else where it is guaranteed such as in ISR
  
  // Work with the value now.
//  Serial.println("button: " + String(reading));
  
}//ISR ENDS





void setup() {
 
  // Configure the pin mode as an input.
  pinMode(buttonInput, INPUT);
  pinMode(alarmPin, OUTPUT);
  
  // Attach an interrupt to the pin, assign the onChange function as a handler and trigger on changes (LOW or HIGH).
  attachInterrupt(buttonInput, ISR,  CHANGE);
  
  Serial.begin(115200);
}

 


 
void loop() {


  while(isPRESSED){    
        
    counter++;
    
    Serial.println(counter);
    if(counter > 3000){
      yield();
      digitalWrite(alarmPin, LOW);
      Serial.println("ALARM OFF");
      counter = 0;
    }
  }
 
}//LOOP ENDS
