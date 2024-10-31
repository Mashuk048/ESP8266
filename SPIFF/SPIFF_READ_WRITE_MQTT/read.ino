
bool readConfigFile() {

  // this opens the config file in read-mode
  File f = SPIFFS.open(CONFIG_FILE, "r");
  
  if (!f) {
    Serial.println("Configuration file not found");
    return false;
  } else {
    // we could open the file
    size_t size = f.size();
    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // Read and store file contents in buf
    f.readBytes(buf.get(), size);
    // Closing file
    f.close();
    // Using dynamic JSON buffer which is not the recommended memory model, but anyway
    // See https://github.com/bblanchon/ArduinoJson/wiki/Memory%20model
    StaticJsonDocument<256> jsondoc;
    deserializeJson(jsondoc, buf.get(), size);//jsonify
    

    // Parse all config file parameters, 
    if (jsondoc.containsKey("did")) {
      String devId = jsondoc["did"];
      Serial.println(devId);
    }

    if (jsondoc.containsKey("ddelay")) {
      unsigned int ddelay = jsondoc["ddelay"];
      Serial.println(ddelay);
    }
    
    if (jsondoc.containsKey("ptime")) {
      unsigned long ptime = jsondoc["ptime"];
      Serial.println(ptime);
    }
  }
  Serial.println("\nConfig file was successfully parsed");
  return true;
}
