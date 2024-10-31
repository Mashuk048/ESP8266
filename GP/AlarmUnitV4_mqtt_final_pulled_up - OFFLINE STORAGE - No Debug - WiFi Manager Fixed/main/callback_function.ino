//-----------------------Callback function-------------------------------------//

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  P_R_I_N_T(topic);
  

  
//-------------------------------------Getting CONFIG data---------------------//

  if(strcmp(topic, configTopic) == 0){
    
      StaticJsonDocument<128> doc;
      deserializeJson( doc,payload,length );
      const char* newdid =  doc["did"];
      unsigned long newStime = doc["stime"];
      int rst = doc["reset"];
      int ap = doc["ap"];
      
      P_R_I_N_T(newdid);
      P_R_I_N_T(newStime); 
      P_R_I_N_T(rst);
      P_R_I_N_T(ap);
    

      if(strcmp(did,newdid) == 0){
        
        //RESET cmd check
        if(rst == 1){
        P_R_I_N_T("RESET");

        char buffer[128];
        doc["reset"] = "success";
        size_t n = serializeJson(doc, buffer);//stringify
        client.publish(responseTopic,buffer, false);
        delay(250);
        ESP.reset();
      }else if(rst == 0){
        P_R_I_N_T("DO NOT RESET");
      }else{
        P_R_I_N_T("INVALID RESET COMMAND");

        char buffer[128];
        doc["reset"] = "failed";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, false);
        delay(250);
      }//rst validation done



      //AP command check
      if(ap == 1){
     
        char buffer[128];
        doc["ap"] = "success";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, false);
        delay(200);

        onDemandAP();
        
      }else if(ap == 0){
        P_R_I_N_T("DO NOT FIRE AP MODE");


      }else{
        char buffer[128];
        doc["ap"] = "failed";
        size_t n = serializeJson(doc, buffer);
        client.publish(responseTopic,buffer, false);
        delay(100);
      }//AP validation done


      //SNOOZE TIME NULL CHECK
      if(newStime != NULL){
        unsigned long snoozeTimeNew = newStime*1000;//in ms

        if(snoozeTimeNew != snoozeTime ){
          writeConfigFile(snoozeTime);
          snoozeTime = snoozeTimeNew;
          Serial.printf("Snooze time updated by %d seconds\n", newStime);
                  
          doc["stime"] = "success"; 
          char buffer[128];
          size_t n = serializeJson(doc, buffer);         
          client.publish(responseTopic, buffer, false);
        }else{
          P_R_I_N_T("Snooze time same as previous");
        }

      }
      else{
        P_R_I_N_T("Invalid time");
        doc["stime"] = "failed";   //notice that using the same json doc
        char buffer[128]; 
        size_t n = serializeJson(doc, buffer);          
        client.publish(responseTopic, buffer, false);
      }//stime validation done
      
    }//did MATCHING DONE
    
    else{
          doc["did"] = "failed";   
          char buffer[128];
          size_t n = serializeJson(doc, buffer);
          P_R_I_N_T("did mismatch");          
          client.publish(responseTopic, buffer, false);
      }
  }//config topic validation ends
     
}//Callback ends
