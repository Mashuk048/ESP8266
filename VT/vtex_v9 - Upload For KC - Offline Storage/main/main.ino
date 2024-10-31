/*
                  OUT TOPIC
Machine running status topic  : dsiot/vt/mrtime
RPM topic                     : dsiot/vt/rpm
Device Response topic         : dsiot/vt/response

                  IN TOPIC
Configuration topic           : dsiot/vt/config
Command topic                 : dsiot/vt/cmd

did : KC002

Command msg format:
{
"did":"VT001",
"reset":"1",
"ap":"1"
}

Config change msg Format: //ptime in second
{
"did":"VT001",
"ptime":"30-600",
"delay":"5-200"
}
*/
//-----------------------------------------------
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Ticker.h>// Ticker for watchdog
Ticker secondTick, thirdTick;
#include "EmonLib.h"//Energy monitor
EnergyMonitor emon1;


//DEVICE NAME
String  did = "HOME";//might change so didn't define as const char*


//MQTT  credentials
const char* mqtt_server = "broker.datasoft-bd.com";
//const char *public_mqtt_server = "mqtt.eclipse.org";
const int mqttPort = 1883;
//int mqttTryCounter=0;



//OUT TOPIC
const char* mrtimeTopic = "dsiot/kc/mrtime";
const char* rpmTopic = "dsiot/kc/rpm";
const char* responseTopic = "dsiot/kc/response";
//IN TOPIC
const char* configTopic = "dsiot/kc/config";
const char* cmdTopic = "dsiot/kc/cmd";
//OFFLINE FILE PATH
const char* CONFIG_FILE = "/config.json";

// Indicates whether ESP has CONFIG DATA saved from previous session
bool initialConfig = false;


//*********************************Default CONFIG values*******************************

// The delay threshold for debounce checking.
unsigned int debounceDelay = 5;//(5ms) works fine for the machine with damaged magnet. Not a const type!
//publish interval
unsigned long publish_interval = 60000;//max = 65,535




WiFiClient espClient;
PubSubClient client(espClient);

//For Timer
unsigned long lastWiFiCheckTime = 0;
unsigned long lastReconnectTime = 0;//MQTT reconnect

unsigned long wifi_check_interval = 5000;

unsigned long lastPublishedTime = 0;







//---------------Defining Pins----------------------------------------------//
#define analogPin A0
#define LED_BUILTIN D0//16 BUILTIN LED
#define STATUS_LED D5//14
const int rpmSensorPin = D1;








//-----------------------------------------RPM----------------------------------------------------//




// Holds the current button state.
volatile int state = 1;

volatile byte interruptCounter = 0;


// Holds the last time debounce was evaluated (in millis).
volatile long lastDebounceTime = 0;



//TIMER FOR RPM
unsigned long lastRpmCalDoneTime = 0;

#define DEBUG 1

#ifdef DEBUG
#define PRINT(x)  Serial.println(x)
#else
#define PRINT(x)
#endif



//--------------------Watchdog----------------------//
volatile int watchdogCount = 0;

void ISRwatchdog() {
  watchdogCount++;
  if (watchdogCount >= 180) {
    PRINT("Watchdog bites!!!");    
    ESP.reset();    
  }
}



// Gets called by the interrupt.
void   ICACHE_RAM_ATTR   ISR() {
  // Get the pin reading.
  int reading = digitalRead(rpmSensorPin);

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
     PRINT(interruptCounter);
  }
  
}//ISR ENDS





//--------------------------------Main Setup----------------------------------------------------//

void setup() {
  spiffRead();
  Serial.begin(115200);
  emon1.current(analogPin, 111.1);   // Current: input pin, calibration.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  pinMode(rpmSensorPin, INPUT);
  attachInterrupt(rpmSensorPin, ISR,  CHANGE);

  secondTick.attach(1, ISRwatchdog);// Attaching ISRwatchdog function
  
  delay(10);
  //set_wifi();
  client.setServer(mqtt_server,mqttPort);//Connecting to broker
  client.setCallback(callback); // Attaching callback for subscribe mode
}
//setup ends




//------------------------------------Main Loop--------------------------------------------------//
void loop(){
  
    watchdogCount = 0;
       
    checkWiFi();
    setMqttReconnectInterval();
    checkIrmsPublishTimer();
    checkRPMTimer();
    
        
}//LOOP ENDS





void checkWiFi(){
    unsigned long now = millis();
    if(now - lastWiFiCheckTime > wifi_check_interval) {
      lastWiFiCheckTime = now;
      
      if (WiFi.status() != WL_CONNECTED){ 
          wifi_manager();
          //set_wifi();
      }
      else {
  //      PRINT(" Wifi already connected");
      }
    }
}


void setMqttReconnectInterval(){      
    if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectTime > 5000) {
          lastReconnectTime = now;
          PRINT("Ticking every 5 seconds for mqtt reconnect");

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


void checkIrmsPublishTimer(){
    //PRINT("Inside sensor data publish");
      
    unsigned long now = millis();
    if(now - lastPublishedTime > publish_interval) {    
        lastPublishedTime = now;
        
        if (!client.connected()){
          reconnect();
        }else{publishIrms();}

    }// Timer ends
}


void publishIrms(){
  StaticJsonDocument<128> doc;
  doc["did"]= did;
  doc["adc"] = anaPRINT_sensor_data();
  char buffer[128];
  size_t n = serializeJson(doc, buffer);
  int result = client.publish(mrtimeTopic, buffer, n);
  delay(250);
  
  if(result){
    PRINT("Irms Published");
    blip();
  }else{
    PRINT("Irms Publish Failed");
  }  
}



double anaPRINT_sensor_data() {
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
//  PRINT((String)"adc_value: "+ Irms);
  return Irms;
}


void checkRPMTimer(){
  unsigned long now = millis();
  if(now - lastRpmCalDoneTime > 60000){
    lastRpmCalDoneTime = now;
    //write code here
    unsigned int rpm = interruptCounter;//the pointer 'ptr' is now pointing to the address of the volatile data byte 'interruptCounter'

    if (!client.connected()){reconnect();}  else  {publishRPM(rpm);}        
    
//    PRINT("RPM = "+rpm);//dereferencing ptr for PRINTing the CONTENT  
    interruptCounter = 0;
  }
}

void publishRPM(unsigned int rpm){
  StaticJsonDocument<128> doc;
  doc["did"]= did;
  doc["rpm"] = rpm;

  char buffer[128];
  size_t n = serializeJson(doc, buffer);
  int result = client.publish(rpmTopic, buffer, n);
  delay(250);  
  
  if(result){
    PRINT("RPM Published");
    blip();
  }else{
    PRINT("RPM Publish Failed");
  }
}





void blip(){    
    digitalWrite(STATUS_LED, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);    
    digitalWrite(STATUS_LED, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
}



void set_wifi() {
  delay(250);
  int tryCount = 0;
  PRINT("");
  PRINT("Connecting to WiFi");
  WiFi.begin("DataSoft_WiFi", "support123");
  while (WiFi.status() != WL_CONNECTED) {
    PRINT(".");
    delay(1000);        //........ 1 sec delay
    PRINT(".");
    tryCount++;
    if (tryCount == 10) return loop(); //exiting loop after trying 10 times
  }
  PRINT("");
  PRINT("Connected");
  PRINT(WiFi.localIP());
  delay(250);
}



void onDemandAP(){
    PRINT("OnDemandAP Starting...");

    // start ticker with 0.5 because we start in AP mode and try to connect
    thirdTick.attach(0.25, toggleLed);
    
  //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //reset settings - for testing
    wifiManager.resetSettings();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    //wifiManager.setTimeout(120);

    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
    //WiFi.mode(WIFI_STA);
    char buff[10];//max 9 digit AP name
    did.toCharArray(buff, 10);
    
    if (!wifiManager.startConfigPortal(buff,"support123")) {
      PRINT("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }

    //if you get here you have connected to the WiFi
    PRINT("connected...yeey :)");
    thirdTick.detach();
}


void spiffRead(){
    // Mount the filesystem
    if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
    
    File file = SPIFFS.open(CONFIG_FILE, "r");
    if(!file){
      Serial.println("Failed to open file for reading");
      return;
    }
    
    Serial.println();
    Serial.println("File Content:");
    while(file.available()){
      Serial.write(file.read());
    }
  
  file.close();
  
}
