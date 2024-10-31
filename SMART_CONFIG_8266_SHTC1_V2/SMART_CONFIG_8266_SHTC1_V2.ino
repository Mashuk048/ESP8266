#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define STATUS_LED 2
//#define INPUT_LED 8 

void setup() {
  int cnt = 0;  

  // set for STA mode
  WiFi.mode(WIFI_STA);
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // led status at pin16
  pinMode(STATUS_LED,OUTPUT);
  
  //configure pin0 
//  pinMode(INPUT_LED, INPUT_PULLUP);

  // deplay for 2 sec for smartConfig
  Serial.println("2 sec before clear SmartConfig");
  delay(2000);
  
  // read pullup
  int isSmartConfig = 0;
  Serial.println(isSmartConfig);
  
  if (isSmartConfig==0) {
    // bink for clear config
    blinkClearConfig();
    Serial.println("clear config");
    // reset default config
    WiFi.disconnect();

  }

  // if wifi cannot connect start smartconfig
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("Waiting for SmartConfig");
    delay(500);
    Serial.print(".");
    if(cnt++ >= 15){
       WiFi.beginSmartConfig();
       while(1){
           delay(500);
           if(WiFi.smartConfigDone()){
             Serial.println("SmartConfig Success");
             blinkSmartConfig();
             break;
           }
       }
    }
  }

  Serial.println("");
  Serial.println("");
  
  WiFi.printDiag(Serial);

  // Print the IP address
  Serial.println(WiFi.localIP());

}


void blinkSmartConfig() {
    digitalWrite(STATUS_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(50);              // wait for a second 
    digitalWrite(STATUS_LED, LOW);    // turn the LED off by making the voltage LOW
    delay(50);
}

void blinkClearConfig() {
  int i=0;
  while(i<=3) {
    digitalWrite(STATUS_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);              // wait for a second 
    digitalWrite(STATUS_LED, LOW);    // turn the LED off by making the voltage LOW
    delay(100);
    i++;
  }
}

void blinkStartConfig() {
    int i=0;
    digitalWrite(STATUS_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
}


void loop() {
 
  
}
