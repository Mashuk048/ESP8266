#include<FS.h>
#include <ArduinoJson.h>



//enter your file name
const char* CONFIG_FILE = "/config.json";
//char buff





void setup() {
    
    Serial.begin(115200);


    if(mountSPIFFS()) {readConfigFile();}  else  {Serial.println("An Error has occurred while mounting SPIFFS");}
}





void loop() {  // put your main code here, to run repeatedly:

}





bool mountSPIFFS(){
    // Mount the filesystem
  bool result = SPIFFS.begin();
  Serial.println("SPIFFS opened: " + result);
  return result;
}



bool readConfigFile() {

//  if(!SPIFFS.begin()){
//    Serial.println("An Error has occurred while mounting SPIFFS");
//    return false;
//  }


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
//      strcpy(thingspeakApiKey, json["did"]);   //overwrite local config variables with parsed values 
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



bool writeConfigFile() {
  Serial.println("Saving config file");
  StaticJsonDocument<256> jsondoc;
  
  // JSONify local configuration parameters
    
  
//  jsondoc["pinSda"] = pinSda;
//  jsondoc["pinScl"] = pinScl;

  // Open file for writing
  File f = SPIFFS.open(CONFIG_FILE, "w");
  if (!f) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  
  // Write data to file and close it
//  json.printTo(f);
  f.close();

  Serial.println("\nConfig file was successfully saved");
  return true;
}
