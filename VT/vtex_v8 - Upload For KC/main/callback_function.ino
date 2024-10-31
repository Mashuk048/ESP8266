//-----------------------Callback function-------------------------------------//

void callback(char* topic, byte* payload, unsigned int length) {
 
  PRINT("Message arrived in topic: ");
  PRINT(topic);
  
//-------------------------------------Getting config data---------------------//

if(strcmp(topic, cmdTopic) == 0){
  
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);

  String devId = doc["did"];
  int rst = doc["reset"];
  int ap = doc["ap"];
  PRINT(devId);
  PRINT(rst);
  PRINT(ap);

  //reset command check
  if( devId == did ){
      if(rst == 1){
        PRINT("RESET");

        char buffer[256];
        doc["status"] = "success";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, n);
        delay(2000);
        ESP.reset();
      }else{
        PRINT("INVALID RESET COMMAND");

        char buffer[256];
        doc["status"] = "failed";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, n);
        delay(2000);
      }

   //AP command check
   if(ap == 1){     
        
        char buffer[256];
        doc["status"] = "success";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, n);
        delay(200);

        onDemandAP();
        
      }else{
        PRINT("INVALID ON-DEMAND AP COMMAND");

        char buffer[256];
        doc["status"] = "failed";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, n);
        delay(2000);
      }
   

      
  }else if( did == "VT000" ){ //MAGIC CODE
      if(rst == 1){
        PRINT("RESET");

        char buffer[256];
        doc["status"] = "success";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, n);
        delay(2000);
        ESP.reset();
      }
      else{
        PRINT("INVALID RESET COMMAND");

        char buffer[256];
        doc["status"] = "failed";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, n);
        delay(2000);
      }
  }else{
      //do nothing
      PRINT("DEVICE ID DID NOT MATCH");
  }
  
}//strcmp ends

  

  if(strcmp(topic, configTopic) == 0){
    
    StaticJsonDocument<256> doc;
    deserializeJson(doc, payload, length);
  
    String devId = doc["did"];
    unsigned int ptime = doc["ptime"];
    unsigned int ddelay = doc["delay"];
    PRINT(devId);
    PRINT(ptime);
    PRINT(ddelay);
   
      
    if( devId == did){
        if(ptime >=10 && ptime <= 600){ //publish time within 10s -10 min
          PRINT("PUBLISH INTERVAL UPDATED");
          publish_interval = ptime*1000;
          delay(2000);
          char buffer[256];
          doc["status"] = "success";
          size_t n = serializeJson(doc, buffer);
          client.publish(responseTopic,buffer, n);
          delay(250);
        }
        else{
          PRINT("INVALID PUBLISH TIME");
          char buffer[256];
          doc["status"] = "failed";
          size_t n = serializeJson(doc, buffer);
          client.publish(responseTopic,buffer, n);
          delay(250);
        }


        if(ddelay >=0 && ddelay <= 200){ //ddelay within 0-200ms
          PRINT("DDELAY TIME UPDATED");
          debounceDelay = ddelay;
          delay(2000);
          char buffer[256];
          doc["status"] = "success";
          size_t n = serializeJson(doc, buffer);
          client.publish(responseTopic,buffer, n);
          delay(250);
        }
        else{
          PRINT("INVALID DDELAY TIME");
          char buffer[256];
          doc["status"] = "failed";
          size_t n = serializeJson(doc, buffer);
          client.publish(responseTopic,buffer, n);
          delay(250);
        }
        
    }else if( did == "VT000" ){
        if(ptime >=10 && ptime <= 600){ //publish interval within 10s - 10 min
          PRINT("PUBLISH INTERVAL UPDATED");
          publish_interval = ptime*1000;
          delay(2000);
          char buffer[256];
          doc["status"] = "success";
          size_t n = serializeJson(doc, buffer);
          client.publish(responseTopic,buffer, n);
          delay(250);
        }
        else{
          PRINT("INVALID PUBLISH TIME");
          char buffer[256];
          doc["status"] = "failed";
          size_t n = serializeJson(doc, buffer);
          client.publish(responseTopic,buffer, n);
          delay(250);
        }

        if(ddelay >=0 && ddelay <= 200){ //ddelay within 0-200ms
          PRINT("DDELAY TIME UPDATED");
          debounceDelay = ddelay;
          delay(1000);
          char buffer[256];
          doc["status"] = "success";
          size_t n = serializeJson(doc, buffer);
          client.publish(responseTopic,buffer, n);
          delay(250);
        }
        else{
          PRINT("INVALID DDELAY TIME");
          char buffer[256];
          doc["status"] = "failed";
          size_t n = serializeJson(doc, buffer);
          client.publish(responseTopic,buffer, n);
          delay(250);
        }
    }else{
         //do nothing
         PRINT("DEVICE ID DID NOT MATCH");
    }
 }//strcomp ends
     
}//Callback ends
