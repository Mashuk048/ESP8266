#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

// DataSoft WiFi Access Point Name
const char* ssid = "DataSoft_WiFi";

// DataSoft WiFi password
const char* password = "support123";

// Mqtt server ip
const char* mqtt_server = "182.48.84.180";

// Create WiFi client
WiFiClient espClient;

// Creates a partially initialised client instance
// Where the WiFi client will pass as argument
PubSubClient client(espClient);

// Connect to WiFi
void setup_wifi() 
{
  // Wait 1 second
  delay(100);
   
  // Connect to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
    
  while (WiFi.status() != WL_CONNECTED) 
  {
    // Wait 1 second
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_pin()
{
  // Use pin 13 for output
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
}

void reconnect() 
{
  // Loop until mqtt client is reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    // if you MQTT broker has clientID,username and password
    // please change following line to 
    // if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      
      //once connected to MQTT broker, subscribe to a topic 
      // Subscribe to topic Data
      client.subscribe("DLES_DATA");
      // Subscribe to topic Lock_Status
      client.subscribe("Lock_Status");
    } 
    else 
    {
      // Indicates the reason of failure
      // -2 means it's failing to create a network connection to the broker
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} 

// Publish relevant status
void publish_status(char* valid_status)
{
  // Mac address valid
  if (valid_status == "Verified")
  {
    client.publish("DLES_MAC", valid_status);
  }
  // Mac address invalid
  if (valid_status == "Unverified")
  {
    client.publish("DLES_MAC", valid_status);
  }
  // Module locked
  if (valid_status == "lock")
  {
    client.publish("Lock_Status", valid_status);
  }
  // Module unlocked
  if (valid_status == "unlock")
  {
    client.publish("Lock_Status", valid_status);
  }
}

// Verify recived Mac address
int check_mac(String mac){

  Serial.print(mac);

  // Check against static mac
  if (mac == "e4:58:b8:69:c5:88" || mac == "E8:B4:C8:CF:DF:E5")
  {
    Serial.println();
    // Valid mac
    return 1;
  }
  else
  {
    Serial.println();
    // Invalid mac
    return 0;
  }
}

// Output pin control that will be to lock/unlock module
int pin_control(String pin_lock_unlock){

  Serial.print(pin_lock_unlock);

  // Lock module
  if (pin_lock_unlock == "0")
  {
    digitalWrite(12, LOW);
    Serial.println();
    return 0;
  }
  // Unlock module
  else
  {
    digitalWrite(12, HIGH);
    Serial.println();
    return 1;
  }
}

// Subscription Callback
// topic - the topic the message arrived on (char*)
// payload - the message payload (byte array)
// length - the length of the message payload (unsigned int)  
// Topic Data
// Topic Lock_Status
void callback(char* topic, byte* payload, unsigned int length) 
{  
  // Indiviual character
  char receivedChar;
  // Contains all recieved character
  String allChar = "";
  int feedback;
  char* valid_status;
  
  for (int i=0;i<length;i++) 
  {
    receivedChar = (char)payload[i];
    // Build up entire message for a particular topic
    allChar.concat(receivedChar);   
  } 

  // Received mac from mobile app
  // Length including :
  if (allChar.length()== 17)
  {
    // Verfiy received mac id
    feedback = check_mac(allChar);
    Serial.print(feedback);

    // Mac from mobile app is matched
    if (feedback == 1)
    {
      // Set status to Verfied
      valid_status = "Verified";
      Serial.println();
      Serial.print(valid_status);
      Serial.println();
      // Publish verified status to be processed by mobile app
      publish_status(valid_status);
    }
    // Anything else from mobile app is not matched
    else
    {
      // Set status to Unverfied
      valid_status = "Unverified";
      Serial.println();
      Serial.print(valid_status);
      Serial.println();
      // Publish unverified status to be processed by mobile app
      publish_status(valid_status);
    }  
  }   

  // Received lock/unlock command from mobile app
  if (allChar.length()== 1)
  {
    // Module lock/unlock feedback
    feedback = pin_control(allChar);
    Serial.print(feedback);

    // Module locked
    if (feedback == 0)
    {
      // Set status to locked
      valid_status = "lock";
      Serial.println();
      Serial.print(valid_status);
      Serial.println();
      // Publish locked status to be processed by mobile app
      publish_status(valid_status);
     }
     else
     {
       // Set status to unlocked
       valid_status = "unlock";
       Serial.println();
       Serial.print(valid_status);
       Serial.println();
       // Publish unlocked status to be processed by mobile app
       publish_status(valid_status);
     }    
   }
}

void setup() {
  
  Serial.begin(115200);

  // Setup pin mode
  setup_pin();
  // Setup wifi connection
  setup_wifi();

  // Connect mqtt client to 
  // 182.163.112.205
  // Port 1883 for connection
  client.setServer(mqtt_server, 1883);
}

void loop() {

  // If mqtt client is not connected
  if (!client.connected()) 
  {
    // Try to connect to mqtt server
    reconnect();
  }
  
  client.loop();

  //  A pointer to a message callback function called when a message arrives for a subscription created by this client
  client.setCallback(callback);
  
}


