int calibrationTime = 30;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 15000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 5;    //the digital pin connected to the PIR sensor's output
int ledPin = 6;

#define RELAY1  7 

/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(RELAY1, OUTPUT);  
  digitalWrite(pirPin, LOW);
  digitalWrite(RELAY1, HIGH);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
  }

////////////////////////////
//LOOP
void loop(){

     if(digitalRead(pirPin) == HIGH){
       digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         digitalWrite(RELAY1,0);
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         Serial.println("Light ON");
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       
       digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           digitalWrite(RELAY1,1);          // Turns Relay Off 
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           Serial.println("Light OFF");
           delay(50);
           }
       }
  }

