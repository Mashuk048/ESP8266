

bool writeConfigFile(const char* did, const char* ddelay, const unsigned long ptime ) {
  Serial.println("Saving config file");
  StaticJsonDocument<256> jsondoc;
  
  // JSONify local configuration parameters
  jsondoc["did"] = did;
  jsondoc["ddelay"] = ddelay;//no need to dereference the pointer to point it to the content
  jsondoc["ptime"] = ptime;
  

  // Open file for writing
  File f = SPIFFS.open(CONFIG_FILE, "w");
  if (!f) {
    Serial.println("Failed to open config file for writing");
    return false;
  }


  // Write data to file and close it
  char buffer[256];
  size_t n = serializeJson(jsondoc, buffer);
  f.print(buffer);
  f.close();

  Serial.println("\nConfig file was successfully saved");
  return true;
}
