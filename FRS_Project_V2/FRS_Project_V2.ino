/*                  (____/ 
 * Use NodeMCU to drive DHT11 and send temperature/humidity value to MQTT server
 * Tutorial URL http://osoyoo.com/2016/11/24/use-nodemcu-to-send-temperaturehumidity-data-to-mqtt-iot-broker/
 * CopyRight John Yu
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const int led = D1; // ONBOARD LED D0 WORKS OPPOSITE


boolean isAlarmOn = false;

// Update these with values suitable for your network.
const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "182.48.84.180";

WiFiClient espClient;
PubSubClient client(espClient);

char msg[5];



unsigned long lastOnTime = 0;

void setup() {
  
  
  pinMode(led,OUTPUT);

  
  delay(100);
  Serial.begin(9600);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}//setup ends here

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  
  checkTime();

}//loop ends here





void setup_wifi() {
    delay(1000);
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
  Serial.print("  Data : ");
  //int p =(char)payload[0]-'0';
  //Serial.println(p);

    // char receivedChar;
    
    


        if (strcmp(topic,"ds/frs/test")==0){

          char receivedChar;
          
          for (int i=0;i<length;i++) {
              receivedChar = (char)payload[i];
              Serial.print(receivedChar);
              

          }

          
        // whatever you want for this topic
             
          if(receivedChar=='0') 
          {
              
              setAlarmOn();

          } 
        
          if(receivedChar=='1')
          {
            
              setAlarmOff();            
          }
          
       

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
      
      client.subscribe("ds/frs/test");
      Serial.println("Subscribed to the topic:    ds/frs");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()


void setAlarmOn(){  //on 0

    digitalWrite(led,HIGH);
    Serial.println("Pin HIGH");
    
    lastOnTime=millis();
    isAlarmOn = true;
    
  
}


void setAlarmOff(){  //on 1
    digitalWrite(led,LOW);
    Serial.println("Pin LOW");
    isAlarmOn = false;
}


void checkTime(){

    if(isAlarmOn){

        unsigned long currentTime = millis();
        //Serial.println(currentTime);
    
        if(currentTime - lastOnTime > 5000){
             //lastTime = currentTime;
             setAlarmOff();
             Serial.println("Timeout");
        }
      
    }

   delay(1000);
  
}


