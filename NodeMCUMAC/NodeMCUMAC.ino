#include <ESP8266WiFi.h>

//const char* ssid = "ssid";
//const char* password = "password";
byte mac[6];

void setup() {
  Serial.begin(9600);
  //WiFi.begin(ssid, password);

  Serial.print("\nStart...\n");
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[0],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.println(mac[5],HEX);
}

void loop() {
  // put your main code here, to run repeatedly:

}
