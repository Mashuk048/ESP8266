#include<FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//enter your file name
const char* CONFIG_FILE = "/config.json";




// Connect to the WiFi
const char* ssid = "DataSoft_WiFi";//DataSoft_WiFi
const char* password = "support123";
const char* mqtt_server = "broker.datasoft-bd.com";
const int mqttPort = 1883;
const char* did = "LAB101";
//mqtt topic
const char* configTopic = "dsiot/gp/config/test";
const char* resTopic = "dsiot/gp/gas/test";


WiFiClient espClient;
PubSubClient client(espClient);


#define DEBUG 1

#if DEBUG
#define  P_R_I_N_T(x)   Serial.println(x)
#else
#define  P_R_I_N_T(x)
#endif




unsigned long ptime;
unsigned long lastReconnectTime = 0;

void setup() {
    
    Serial.begin(115200);

    setup_wifi();
    setup_mqtt();

    if(mountSPIFFS()){      readConfigFile();    }else{Serial.println("An Error has occurred while mounting SPIFFS");}
}



void loop() {  // put your main code here, to run repeatedly:
  setMqttReconnectInterval();
}

bool mountSPIFFS(){
    // Mount the filesystem
  bool result = SPIFFS.begin();
  Serial.println("SPIFFS opened: " + result);
  return result;
}




void setMqttReconnectInterval(){      
    if (!client.connected()) {
      
      unsigned long now = millis();
      if (now - lastReconnectTime > 5000) {
          lastReconnectTime = now;
          P_R_I_N_T("Ticking every 5 seconds");
          

//          P_R_I_N_T(WiFi.status());

          if(WiFi.status() == WL_CONNECTED){
              // Attempt to mqtt reconnect
              if (reconnect()) {     
                
                lastReconnectTime = 0;//GOOD Why???
              }
          }
          
//       if(WiFi.status() != WL_CONNECTED){toggleTicker.attach(0.25, toggleLed);}//end of WL_CONNECTED       
 
      }//end of interval check
   }else{//if client connected
      client.loop();
   }
  
}//setMqttReconnect



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
