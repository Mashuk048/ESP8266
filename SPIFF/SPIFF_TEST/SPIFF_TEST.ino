#include<ESP8266WiFi.h>
#include<FS.h>


//enter your file name
const char* file="/config.json";



void setup(){
    delay(1000);
    Serial.begin(115200);
    
    Serial.println();
    
    //Initialize File system
    SPIFFS.begin();
    Serial.println("File System Initialized");
    //open file for reading
    File dataFile=SPIFFS.open(file,"r"); 
    Serial.println("Reading Data from File:");
    //Data from file
    //Read upto complete size
    for(int i=0;i<dataFile.size();i++)
    {
      Serial.print((char)dataFile.read());
    }
    dataFile.close();
}


void loop()
{
}
