/*
                  OUT TOPIC
Machine running status topic  : dsiot/vt/mrtime
RPM topic                     : dsiot/vt/rpm
Device Response topic         : dsiot/vt/response

                  IN TOPIC
Configuration topic           : dsiot/vt/config
Command topic                 : dsiot/vt/cmd

did : VT001

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


// Ticker for watchdog

#include <Ticker.h>
Ticker secondTick, thirdTick;


//Energy monitor
#include "EmonLib.h"
EnergyMonitor emon1;


//DEVICE NAME
String  did = "LAB";//might change so didn't define as const char*


//MQTT  credentials
const char* mqtt_server = "broker.datasoft-bd.com";
//const char *public_mqtt_server = "mqtt.eclipse.org";
const int mqttPort = 1883;
//int mqttTryCounter=0;



//OUT TOPIC
const char* mrtimeTopic = "dsiot/lab/mrtime";
const char* rpmTopic = "dsiot/lab/rpm";
const char* responseTopic = "dsiot/lab/response";
//IN TOPIC
const char* configTopic = "dsiot/lab/config";
const char* cmdTopic = "dsiot/lab/cmd";



WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastWiFiCheckTime = 0;
unsigned long lastReconnectTime = 0;//MQTT reconnect

unsigned long wifi_check_interval = 5000;

unsigned long lastPublishedTime = 0;

//publish interval
unsigned int publish_interval = 60000;//max = 65,535

//unsigned long previousMillis2 = 0;
//long deep_sleep_interval = 65000;



//---------------Defining Pins----------------------------------------------//
#define analogPin A0
#define LED_BUILTIN D0//16 BUILTIN LED
#define STATUS_LED D5//14
const int rpmSensorPin = D1;





//--------------------Watchdog----------------------//
volatile int watchdogCount = 0;

void ISRwatchdog() {
  watchdogCount++;
  if (watchdogCount >= 180) {
    Serial.println("Watchdog bites!!!");    
    ESP.reset();    
  }
}



//-----------------------------------------RPM----------------------------------------------------//




// Holds the current button state.
volatile int state = 1;

volatile byte interruptCounter = 0;


// Holds the last time debounce was evaluated (in millis).
volatile long lastDebounceTime = 0;

// The delay threshold for debounce checking.
unsigned int debounceDelay = 30;//(5ms) works fine for the machine with damaged magnet. Not a const type!


//TIMER FOR RPM
unsigned long lastRpmCalDoneTime = 0;






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
     Serial.println(interruptCounter);
  }
  
}//ISR ENDS





//--------------------------------Main Setup----------------------------------------------------//

void setup() {
  Serial.begin(115200);
  emon1.current(A0, 111.1);   // Current: input pin, calibration.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  pinMode(rpmSensorPin, INPUT_PULLUP);
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
  //      Serial.println(" Wifi already connected");
      }
    }
}


void setMqttReconnectInterval(){      
    if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectTime > 5000) {
          lastReconnectTime = now;
          Serial.println("Ticking every 5 seconds for mqtt reconnect");

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
    //Serial.println("Inside sensor data publish");
      
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
  doc["adc"] = analog_sensor_data();
  char buffer[128];
  size_t n = serializeJson(doc, buffer);
  int result = client.publish(mrtimeTopic, buffer, n);
  delay(250);
  
  if(result){
    Serial.println("Irms Published");
    blip();
  }else{
    Serial.println("Irms Publish Failed");
  }  
}



double analog_sensor_data() {
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
//  Serial.println((String)"adc_value: "+ Irms);
  return Irms;
}


void checkRPMTimer(){
  unsigned long now = millis();
  if(now - lastRpmCalDoneTime > 60000){
    lastRpmCalDoneTime = now;
    //write code here
    unsigned int rpm = interruptCounter;//the pointer 'ptr' is now pointing to the address of the volatile data byte 'interruptCounter'

    if (!client.connected()){reconnect();}  else  {publishRPM(rpm);}        
    
//    Serial.println("RPM = "+rpm);//dereferencing ptr for printing the CONTENT  
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
    Serial.println("RPM Published");
    blip();
  }else{
    Serial.println("RPM Publish Failed");
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
  Serial.println("");
  Serial.println("Connecting to WiFi");
  WiFi.begin("DataSoft_WiFi", "support123");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);        //........ 1 sec delay
    Serial.print(".");
    tryCount++;
    if (tryCount == 10) return loop(); //exiting loop after trying 10 times
  }
  Serial.println("");
  Serial.println("Connected");
  Serial.println(WiFi.localIP());
  delay(250);
}

void onDemandAP(){
    Serial.println("OnDemandAP Starting...");

    // start ticker with 0.5 because we start in AP mode and try to connect
    thirdTick.attach(0.25, tick);
    
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
    
    if (!wifiManager.startConfigPortal(buff)) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    thirdTick.detach();
}
