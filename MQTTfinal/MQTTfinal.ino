/*                  (____/ 
 * Use NodeMCU to drive DHT11 and send temperature/humidity value to MQTT server
 * Tutorial URL http://osoyoo.com/2016/11/24/use-nodemcu-to-send-temperaturehumidity-data-to-mqtt-iot-broker/
 * CopyRight John Yu
 */
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <dht.h>
dht DHT;

const int ledPin1 = 1; // ONBOARD LED WORKS OPPOSITE
const int ledPin2 = 2; // Pin
const int ledPin3 = 3; // 


// Define NodeMCU D3 pin to as temperature data pin of  DHT11
#define DHT11_PIN D3

// Update these with values suitable for your network.
const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "182.163.112.219";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[20];
int value = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin1,OUTPUT);
  pinMode(ledPin2,OUTPUT);
  pinMode(ledPin3,OUTPUT);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
//  int chk = DHT.read11(DHT11_PIN);
//  Serial.print(" Starting Humidity: " );
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  
  long now = millis();
  // read DHT11 sensor every 2 seconds
  if (now - lastMsg > 5000) {
     lastMsg = now;
     //int led0 = DHT.read11(DHT11_PIN);
     String msg="1";     //msg = msg+"%";     //msg=msg+"1";  doesn't work cause integer
     char message[20];
     msg.toCharArray(message,20);
     Serial.println(message);
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
  Serial.print("Topic : ");
  Serial.print(topic);
  Serial.print(" Data : ");
  int p =(char)payload[0]-'0';
  Serial.println(p);


  if (strcmp(topic,"ds/smartlife/app")==0){
    // whatever you want for this topic
          // if MQTT comes a 0 message, show humidity
      if(p==0) 
      {
        digitalWrite(ledPin1, LOW); // 
        Serial.println("ledPin0=OFF");
      } 
      // if MQTT comes a 1 message, show temperature
      if(p==1)
      {
       digitalWrite(ledPin1, HIGH);
       Serial.println("ledPin0=ON");
        
      }
        if(p==2)
      {
       digitalWrite(ledPin2, LOW);
       Serial.println("ledPin1=OFF");
        
      }
    
        if(p==3)
      {
       digitalWrite(ledPin2, HIGH);
       Serial.println("ledPin1=ON");
        
      }
          if(p==4)
      {
       digitalWrite(ledPin3, LOW);
       Serial.println("ledPin1=ON");
        
      }
          if(p==5)
      {
       digitalWrite(ledPin3, HIGH);
       Serial.println("ledPin1=ON");
      }
  }
 
  if (strcmp(topic,"ds/smartlife/intensity")==0) {
    
       //digitalWrite(ledPin3, HIGH);
       Serial.print("pwm : ");
       Serial.println(p);
     
    
  }
 
  if (strcmp(topic,"blue")==0) {
    // this one is blue...
  }  
 
  if (strcmp(topic,"green")==0) {
    // i forgot, is this orange?
  }  

   Serial.println();

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
      client.subscribe("ds/smartlife/app");
      Serial.println("Subscribed to the topic:    ds/smartlife/app");
      client.subscribe("ds/smartlife/intensity");
      Serial.println("Subscribed to the topic:    ds/smartlife/intensity");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

