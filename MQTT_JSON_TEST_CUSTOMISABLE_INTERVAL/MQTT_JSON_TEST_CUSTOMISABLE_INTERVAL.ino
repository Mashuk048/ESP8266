#include <ESP8266WiFi.h>
//#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED_BUILTIN 2 // ONBOARD LED WORKS OPPOSITE


// Update these with values suitable for your network.
const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "182.163.112.219";

WiFiClient espClient;
PubSubClient client(espClient);

//global timer variables
unsigned  long lastMsgTime = 0;
long INTERVAL = 3000;


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

  setPublishTimer();
}


void setPublishTimer(){
  
    long now = millis();
  // read DHT11 sensor every 2 seconds
  if (now - lastMsgTime > INTERVAL) {
     lastMsgTime = now;

      Serial.printf("Ticking every %i seconds\n", INTERVAL/1000); //3=interval
      
     //int led0 = DHT.read11(DHT11_PIN);
     String msg="1";     //msg = msg+"%";     //msg=msg+"1";  doesn't work cause integer
     char message[20];
     msg.toCharArray(message,20);
//     Serial.println(message);
     //publish sensor data to MQTT broker
    //client.publish("Seyam/data", message);
    
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


void callback(char* topic, byte* payload, unsigned int length) 
{

  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);

  String did = doc["did"];
  int cmd = doc["cmd"];
  Serial.println(did);
  Serial.println(cmd);

  if( did == "101"){
      if(cmd == 1){
        Serial.println("RESET");
        client.publish("dev/res","success");
        delay(1000);
      }
      else{
        Serial.println("COMMAND INVALID");
        client.publish("dev/res","failed");
        delay(1000);
      }
  }else if( did == "000" ){
      if(cmd == 1){
        Serial.println("RESET");
        client.publish("dev/res","success");
        delay(1000);
      }
      else{
        Serial.println("COMMAND INVALID");
        client.publish("dev/res","failed");
        delay(1000);
      }
  }else{
      
  }

  
   
//  Serial.println();

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
      client.subscribe("ds/reset");
      Serial.println("Subscribed to the topic:    ds/reset");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()
