//-----------------------Callback function-------------------------------------//

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  
//-------------------------------------Getting config data---------------------//
StaticJsonDocument<128> myJsonDoc;
deserializeJson( myJsonDoc,payload,length );
const char* new_did =  myJsonDoc["did"];
const char* new_ddelay =  myJsonDoc["ddelay"];
unsigned long new_ptime = myJsonDoc["ptime"];


Serial.println(new_did);
Serial.println(new_ddelay);
Serial.println(new_ptime);

if(strcmp(did,new_did) == 0){
  if(new_ddelay != NULL && new_ptime != NULL){
    ptime = new_ptime*1000;//in ms

    writeConfigFile(did, new_ddelay, ptime );
    
    Serial.printf("Publish time updated by %d seconds\n", new_ptime);
    myJsonDoc["status"] = "success"; 
    char buffer[128];   
    size_t n = serializeJson(myJsonDoc, buffer);
    Serial.print("Published:");
    P_R_I_N_T(client.publish(resTopic, buffer, n));    
  }
  else{
    P_R_I_N_T("Invalid publish time");
    
//    StaticJsonDocument<128> doc;
//    doc["did"] = did;

    myJsonDoc["status"] = "failed";   
    char buffer[128]; 
    size_t n = serializeJson(myJsonDoc, buffer);
    Serial.print("Published:");
    P_R_I_N_T(client.publish(resTopic, buffer, n));
  }  
}

else{
    myJsonDoc["status"] = "failed";   
    char buffer[128];
    size_t n = serializeJson(myJsonDoc, buffer);
    Serial.print("Published:");
    P_R_I_N_T(client.publish(resTopic, buffer, n));
}

     
}//Callback ends
