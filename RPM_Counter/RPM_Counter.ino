/*
 Since my debouncing code works on interrupt on CHANGE, so whenever it was missing the second interrupt(change of level shifting), it would make the "state" variable remain stuck as TRUE and long press detection loop runs endlessly making the device unstable.
 So to solve this, here i am forcefully altering the state to false=0(Depressed) inside the long press detecton loop to stop it from running endlessly.
 Device behaviour: Now any infirm or improper button press will set off the long press detection loop and once counter reaches the threshold(after 3 seconds), it stops the siren that was mistakenly fired up.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


// Connect to the WiFi
const char* ssid = "DataSoft_WiFi";//DataSoft_WiFi
const char* password = "support123";
const char* mqtt_server = "broker.datasoft-bd.com";
const int mqttPort = 1883;
const char* did = "VT101";

//mqtt topic
const char* ivalTopic = "dsiot/vt/ival";
const char* resTopic = "dsiot/vt/rpm";

#define DEBUG 1

#if DEBUG
#define  P_R_I_N_T(x)   Serial.println(x)
#else
#define  P_R_I_N_T(x)
#endif



//MQTT
unsigned long lastReconnectTime = 0;
WiFiClient espClient;
PubSubClient client(espClient);



#define ledPin 2 //D4  ONBOARD LED
//const int sensorPin = 14;//D5
const int sensorPin = D1;//D5




// Holds the current button state.
volatile int state = 1;

//
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
//    if(isAlarming){P_R_I_N_T("Already Alarming!");}else{setAlarm_and_Publish("SOS",1);}
      interruptCounter++;
      P_R_I_N_T(interruptCounter);

  }
  
// Work with the value now.
//  Serial.println("button: " + String(reading));
  
}//ISR ENDS




void setup() {
    
    
    pinMode(sensorPin, INPUT_PULLUP);
  
  
    attachInterrupt(sensorPin, ISR,  CHANGE);
    Serial.begin(115200);
    P_R_I_N_T("READY");
    delay(500);
    
    
    setup_wifi();
    setup_mqtt();
}


void loop() {

//    while(!state){          
//      setLongPressTimer();
//      yield();
//    }

//    if(SNOOZE){checkResumeTimer();}else{readRadioData();};
    
    
    setMqttReconnectInterval();
    setRPMTimer();
    
}//LOOP ENDS


void setRPMTimer(){
  unsigned long now = millis();
  if(now - lastRpmCalDoneTime > 60000){
    lastRpmCalDoneTime = now;
    //write code here
    Serial.println(interruptCounter);
    delay(100);
    interruptCounter = 0;    
  }  
}


void setup_wifi() {
    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    P_R_I_N_T(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  P_R_I_N_T("");
  P_R_I_N_T("WiFi connected");
  P_R_I_N_T("IP address: ");
  P_R_I_N_T(WiFi.localIP());
}

void setup_mqtt(){      
   client.setServer(mqtt_server, mqttPort);
   client.setCallback(callback);
}



void setAlarm_and_Publish(const char *type, const int state){
  
  
  digitalWrite(ledPin, state);
  
  StaticJsonDocument<128> doc;
  char buffer[128];
  doc["did"] = did;
  doc["alarm"] = state;
  doc["type"] = type;
  size_t n = serializeJson(doc, buffer);
  Serial.print("Published:");
  P_R_I_N_T(client.publish(resTopic, buffer, n));
  delay(250);
  
}


void setMqttReconnectInterval(){      
    if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectTime > 5000) {
          lastReconnectTime = now;
          P_R_I_N_T("Ticking every 5 seconds");

          //write your own code

//          P_R_I_N_T(WiFi.status());

          if(WiFi.status() == WL_CONNECTED){
              // Attempt to mqtt reconnect
              if (reconnect()) {
                lastReconnectTime = 0;//GOOD
              }
          }
        
      }//end of interval check
   }else{
      client.loop();
   }
  
}//setMqttReconnect




//
//
//void checkResumeTimer(){
//  unsigned long now = millis();
//  if(now - lastStopTime > stime){
//      SNOOZE = false;//resume radio
//  }
//}

