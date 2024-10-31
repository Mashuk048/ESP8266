// Holds the current button state.
volatile int state;

// Holds the last time debounce was evaluated (in millis).
volatile long lastDebounceTime = 0;

// The delay threshold for debounce checking.
const int debounceDelay = 50;

// pin is 2.
const int   buttonInput = 12;

//#define ledPin 0






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

  // Work with the value now.
  Serial.println("button: " + String(reading));
}





void setup() {
 
  // Configure the pin mode as an input.
  pinMode(buttonInput, INPUT);
  
  // Attach an interrupt to the pin, assign the onChange function as a handler and trigger on changes (LOW or HIGH).
  attachInterrupt(buttonInput, ISR,  CHANGE);
  
  Serial.begin(115200); 
}

 
//void handleInterrupt() {
//  interruptCounter++;
//}



 
void loop() {
 
}
