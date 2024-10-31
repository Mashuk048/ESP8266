/*                  (____/ 
 * zakir.hasan@datasoft-bd.com
 *
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <dht.h>
//dht DHT;

#define ledPin1 D5 // ONBOARD LED WORKS OPPOSITE
#define ledPin2 D6 // Pin
#define ledPin3 D7 // 
#define sensorIn A0

double Power;

int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module

double VRMS;
double AmpsRMS;
double Watts;
float result;

int readValue;             //value read from the sensor
int maxValue = 0;          // store max value here
int minValue = 1024;       // store min value here

// Define NodeMCU D3 pin to as temperature data pin of  DHT11
//#define DHT11_PIN D3

// Update these with values suitable for your network.
const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
//const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* mqtt_server = "182.48.84.180";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsgTime = 0;
char msg[5];
int dutycycle=0;
//int value = 0;

void setup() {
  
  
  pinMode(ledPin1,OUTPUT);
  pinMode(ledPin2,OUTPUT);
  pinMode(ledPin3,OUTPUT);

  analogWrite(ledPin1, 0);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  
  analogWriteFreq(500); //Set PWM frequency 500, default is 1000
  analogWriteRange(100);  //Set range 0~255, default is 0~1023
  //delay(100);
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

  Power = getVPP();
  
//  long now = millis();
//  // read DHT11 sensor every given seconds
//  if (now - lastMsgTime > 5000) {
//     lastMsgTime = now;
//     
//     //Write code here
//      
//     //Serial.println(dutycycle);
//  }
//  
}//loop ends here





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
  Serial.print("  Data : ");
//  int p =(char)payload[0]-'0';
//  Serial.println(p);

    // char receivedChar;
    
        if (strcmp(topic,"ds/sl/st")==0) {

            sendStatus();              
             
        }  
     


        if (strcmp(topic,"ds/sl/app")==0){

          char receivedChar;
          
          for (int i=0;i<length;i++) {
              receivedChar = (char)payload[i];
              Serial.print(receivedChar);
              
            //  if (receivedChar == '0'){
            //    Serial.println("Got 0");
            //  }
            //  
            //  if (receivedChar == '1')
            //   Serial.println("Got 1");
          }

          
        // whatever you want for this topic
              // if MQTT comes a 0 message, show humidity
          if(receivedChar=='0') 
          {
            analogWrite(ledPin1, 0); // 
            Serial.println(" device1=OFF");
          } 
          // if MQTT comes a 1 message, show temperature
          if(receivedChar=='1')
          {
           analogWrite(ledPin1, 100);
           Serial.println(" device1=ON");
            
          }
            if(receivedChar=='2')
          {
           digitalWrite(ledPin2, LOW);
           Serial.println(" device2=OFF");
            
          }
        
            if(receivedChar=='3')
          {
           digitalWrite(ledPin2, HIGH);
           Serial.println(" device2=ON");
            
          }
              if(receivedChar=='4')
          {
           digitalWrite(ledPin3, LOW);
           Serial.println(" device3=OFF");
            
          }
              if(receivedChar=='5')
          {
           digitalWrite(ledPin3, HIGH);
           Serial.println(" device3=ON");
          }
      }


      if (strcmp(topic,"ds/sl/i")==0) {
        
            char pwm;
            //message_buff[i] = payload[i];
            for (int i=0;i<length;i++) {
                pwm  = (char)payload[i];
                Serial.print(pwm);


             //if(!(analogRead(D1)==0)){
                if (pwm == '0'){
                    dutycycle=10;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 10");
                }
                
                if (pwm == '1'){
                    dutycycle=20;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 20");
                }
                if (pwm == '2'){
                    dutycycle=30;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 30");
                }
                if (pwm == '3'){
                    dutycycle=40;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 40");
                }
                if (pwm == '4'){
                    dutycycle=50;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 50");
                }
                if (pwm == '5'){
                    dutycycle=60;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 60");
                }
                if (pwm == '6'){
                    dutycycle=70;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 70");
                }
                if (pwm == '7'){
                    dutycycle=80;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 80");
                }
                if (pwm == '8'){
                    dutycycle=90;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 90");
                }
                if (pwm == '9'){
                    dutycycle=100;
                    analogWrite(ledPin1, dutycycle);
                    Serial.println("lumen 100");
                }
             //}

            }
            //Serial.print(" pwm : ");
            //Serial.print(pwm);
            
                          
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
      client.subscribe("ds/sl/app");
      Serial.println("Subscribed to the topic:    ds/sl/app");
      client.subscribe("ds/sl/i");
      Serial.println("Subscribed to the topic:    ds/sl/i");
      client.subscribe("ds/sl/st");
      Serial.println("Subscribed to the topic:    ds/sl/cs");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()



void sendStatus(){
           //int led0 = DHT.read11(DHT11_PIN);
           //if((analogRead(D1) > 0)){     //analogRead fucntion won't work for reading pwm dutycycle 

             //String msg="3";     //msg = msg+"%";     //msg=msg+"1";  doesn't work cause integer
             String msg = String(dutycycle);
             char message[20];
             msg.toCharArray(message,20);
             
             //publish sensor data to MQTT broker
             client.publish("ds/sl/cs", message);
             Serial.print(message);
             Serial.println(" published"); 
            

         

           if((digitalRead(ledPin2) == HIGH)){

             String msg="3";     //msg = msg+"%";     //msg=msg+"1";  doesn't work cause integer
             char message[20];
             msg.toCharArray(message,20);
             
             //publish sensor data to MQTT broker
             client.publish("ds/sl/cs", message);
             Serial.print(message);
             Serial.println(" published"); 
            
           }

           if((digitalRead(ledPin3) == HIGH)){

             String msg="5";     //msg = msg+"%";     //msg=msg+"1";  doesn't work cause integer
             char message[20];
             msg.toCharArray(message,20);
             
             //publish sensor data to MQTT broker
             client.publish("ds/sl/cs", message);
             Serial.print(message);
             Serial.println(" published"); 
            
           }    
  
}



double getVPP()
{
    //Serial.println("I'm here ");
   
   //long start_time = millis();
   long now = millis();
   if((now - lastMsgTime) > 5000) //sample for 1 Sec
   {
       lastMsgTime = now;
       //Write code here
       readValue = analogRead(sensorIn);
       //Serial.println(readValue);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       else if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }

          // Subtract min from max
       result = ((maxValue - minValue) * 3.4)/1024.0;
       //Serial.println(result);
       
       VRMS = (result/2.0) * 0.707; 
       AmpsRMS = (VRMS * 1000)/mVperAmp;
       
       if(AmpsRMS<.17){
        Watts=0;
        Serial.print(Watts);
        Serial.println(" W");
       }else{
         Watts=(AmpsRMS)*210;
        Serial.print(Watts);
        Serial.println(" W");
       }  

       sendWatt();
       
   }
        
   return Watts;
 }


 void sendWatt(){

             String msg = String(Watts);
             char message[20];
             msg.toCharArray(message,20);
             
             //publish sensor data to MQTT broker
             client.publish("ds/sl/wt", message);
             Serial.print(message);
             Serial.println(" published");
  
  
}

