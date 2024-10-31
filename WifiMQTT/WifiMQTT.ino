//#include <EEPROM.h>
#include <WiFi.h>
#include <PubSubClient.h>
//#include <Wire.h>
//#include <Adafruit_INA219.h>
 
// Connect to the WiFi
const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "test.mosquitto.org";

//global timer variables
unsigned long previousMillis = 0;
long interval = 3000;
int a = 60;

WiFiClient espClient;
PubSubClient client(espClient);
 
const int ledPin = 1; // Pin with LED on Adafruit Huzzah



void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i=0;i<length;i++) {
  char receivedChar = (char)payload[i];
  Serial.print(receivedChar);
  if (receivedChar == '0')
  // ESP8266 Huzzah outputs are "reversed"
  digitalWrite(ledPin, HIGH);
  if (receivedChar == '1')
   digitalWrite(ledPin, LOW);
  }
  Serial.println();
}
 
 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("ESP8266 Client")) {
  Serial.println("connected");
  // ... and subscribe to topic
  client.subscribe("Seyam/led");
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}

 
void setup()
{
 Serial.begin(9600);
 
//For WIFI

  setup_wifi();

 //For MQTT
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
 
 pinMode(ledPin, OUTPUT);
}


 
void loop()
{

  setTimer();

  
 if (!client.connected()) {
  reconnect();
 }
 client.loop();
}




void setTimer(){
  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
   previousMillis = currentMillis;
   Serial.println("Ticking every 3 seconds"); //2=interval
   Serial.println(previousMillis);
   Serial.println(a);

      String LoRaData="Hi";
      char msgBuff[64];
      LoRaData.toCharArray(msgBuff, 64); //getBytes does the same thing
      int result = client.publish ("lora/data", msgBuff);
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

