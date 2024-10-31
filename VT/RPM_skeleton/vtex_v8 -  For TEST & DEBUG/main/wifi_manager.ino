//for LED status
Ticker ticker;

void tick()
{
  int state = digitalRead(LED_BUILTIN);  // get the current state of GPIO1 pin
  digitalWrite(LED_BUILTIN, !state);     // set pin to the opposite state
  digitalWrite(STATUS_LED, !state);     // set pin to the opposite state
}


void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.25, tick);
}


void wifi_manager(){
  
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.5, tick);
  Serial.println("AP Mode");
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  char buff[10];//max 9 digit AP name
  did.toCharArray(buff, 10);
  
  wifiManager.autoConnect(buff);//START AP AS DEVICE ID NAME

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  
  turnOffBlinking();  
}

void turnOffBlinking(){
    digitalWrite (LED_BUILTIN, LOW);
    digitalWrite(STATUS_LED, HIGH);
}
