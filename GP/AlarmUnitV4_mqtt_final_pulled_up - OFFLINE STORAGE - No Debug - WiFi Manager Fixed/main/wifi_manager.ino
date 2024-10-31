
//for power and net status
void toggleLed()
{
//  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));     // set pin to the opposite state
  
  digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
}

void alwaysOnLed(){
//    digitalWrite (LED_BUILTIN, LOW);
    digitalWrite(STATUS_LED, HIGH);
}


void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, PRINT it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  toggleTicker.attach(0.25, toggleLed);
}


void wifi_manager(){
  
    // start ticker with 0.5 because we start in AP mode and try to connect
  toggleTicker.attach(1, toggleLed);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setConfigPortalTimeout(30);
  
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);


  char buff[10];//max 9 digit AP name
//  did.toCharArray(buff, 10);
  strcpy(buff, did);
  
  if (!wifiManager.autoConnect(buff, "support123")) {//START AP AS DEVICE ID NAME
    P_R_I_N_T("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
     ESP.reset();
//   ESP.deepSleep(5000000);

     delay(5000);
  }

  //ELSE if you get here you have connected to the WiFi
  P_R_I_N_T("connected...yeey :)");
  
  delay(250);
  
//  toggleTicker.detach();  //shift inside mqtt reconnect
//  alwaysOnLed();
}
