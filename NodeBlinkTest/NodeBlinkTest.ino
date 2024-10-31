/*
 * For NodeMCU (8266) both D1 and 5 refer to the same pin
 * For others, such as "Generic ESP8266 Module", it will
 * report error of: 'D1' was not declared in this scope.
 */
const int io5 = D1;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pins as an output.
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println(LED_BUILTIN);
  //pinMode(D1, OUTPUT);
  pinMode(io5, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
    
  digitalWrite(io5, HIGH);    //or  digitalWrite(D1, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  
  
  digitalWrite(io5, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
