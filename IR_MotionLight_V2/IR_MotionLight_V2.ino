#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define sensorPin D5
#define relay D6

int buttonState = 0;


// Connect to the WiFi
const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
const char* mqtt_server = "test.mosquitto.org";
const int mqttPort = 1883;
String did = "RA101";


unsigned long lastReconnectTime = 0;


boolean isOnStatePublished = false;
boolean isOffStatePublished = false;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 
  Serial.begin(115200);
  Serial.println("Ready");
  pinMode(sensorPin, INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);


   setup_wifi();

  //For MQTT
 client.setServer(mqtt_server, mqttPort);
 client.setCallback(callback);
}
 

 
void loop() {
  
    buttonState = digitalRead(sensorPin);
    
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (buttonState == HIGH) {
      // turn LED on:
      digitalWrite(relay, HIGH);

      if(isOnStatePublished == false){
        Serial.println("OFF published");
//        client.publish("motion/response", "1");
//        delay(500);
        isOnStatePublished = true;
        isOffStatePublished = false;
      }
    }
    else {
      // turn LED off:
      digitalWrite(relay, LOW);
      
        if(isOffStatePublished == false){
        Serial.println("ON published");
//        client.publish("motion/response", "1");
//        delay(500);
        isOffStatePublished = true;
        isOnStatePublished = false;
      }
    }


    setMqttReconnectTimer();

}

void setMqttReconnectTimer(){
  if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectTime > 5000) {
        lastReconnectTime = now;
        Serial.println("Ticking every 5 seconds");
        // Attempt to reconnect
        if (reconnect()) {
          lastReconnectTime = 0;//GOOD
        }
      }
   }else{
      client.loop();
   }   
}



  
void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
