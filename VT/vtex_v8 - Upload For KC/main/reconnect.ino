//------------------------------------------------While client not conncected---------------------------------//


boolean reconnect() {

    PRINT("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);
  
    if (client.connect(clientId.c_str())) {
      PRINT("connected");

      alwaysOnLed();

      if(client.subscribe(configTopic)){
        PRINT("Subscribed to configTopic");
      }
      
      delay(200);
      
      if(client.subscribe(cmdTopic)){
        PRINT("Subscribed to cmdTopic"); 
      }
      
      delay(200);
      
    }else{  
      PRINT("failed, rc=");
      PRINT(client.state());
      PRINT(" try again");
//      mqttTryCounter++;
//      PRINT(mqttTryCounter);
      // Wait 6 seconds before retrying
//      delay(5000); //blocking
    }
  return client.connected();
}// reconnect() ends
