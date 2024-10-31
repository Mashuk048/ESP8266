#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define LED_BUILTIN 2 // ONBOARD LED WORKS OPPOSITE

//For storing sensor value
float volt;
float current;
float power;

String data="";


// Update these with values suitable for your network.
const char* ssid = "Gourab";
const char* password = "87654321";
char* topic = "channels/1017675/publish/R9UN8DNLAVWWCLU9"; 
const char* mqtt_server = "mqtt.thingspeak.com";

WiFiClient espClient;
PubSubClient client(espClient);


unsigned long lastPublishTime = 0;




void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
  
  digitalWrite(LED_BUILTIN, LOW);
  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  readDataFromSerial();
  
  setPublishTimer();
}


void readDataFromSerial(){
  
  if(Serial.available()){

    while (Serial.available()) {
        data = Serial.readString();
    }
    Serial.println(data);
    jsonify(data);   
  }
}




void setPublishTimer(){
  
  unsigned long now = millis();
  
  if (now - lastPublishTime > 30000) {
     lastPublishTime = now;
     Serial.println("Ticking every 30");
     //Write your code here

     String payload="field1=";
            payload+=volt;
            payload+="&field2=";
            payload+=current;//90% humidity
            payload+="&field3=";
            payload+=power;//90% humidity
            payload+="&status=MQTTPUBLISH";
  
     //publish sensor data to thingspeak MQTT broker
     if(client.publish(topic, payload.c_str())){
        Serial.println("all data publish success");
     };
  }

  
}



//Assuming that data is coming in this format:  {"v":"220.2","i":"5.88","p":"120.55"}

void jsonify(const String payload){
      StaticJsonDocument<192> doc;
      
      deserializeJson( doc, payload.c_str());
      volt =  doc["Voltage_V_sensor"];
      current = doc["Current"];
      power = doc["Power"];
      Serial.println(volt);
      Serial.println(current);
      Serial.println(power);
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


void callback(char* topic, byte* payload, unsigned int length) 
{

} //end callback







void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()
