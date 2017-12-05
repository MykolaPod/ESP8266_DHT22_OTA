#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <DHT.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define DHTPIN 2
#define DHTTYPE DHT22
bool _isWiFiConnected = false;
char* _wifi_SSID = "";
char* _wifi_password = "";

ESP8266WiFiMulti WiFiMulti;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(10);

  bool connectionResult = tryConnectToWifi(_wifi_SSID,_wifi_password);
  if(connectionResult){
    ArduinoOTA.setHostname("EgoESP1");
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();    
  }
  dht.begin();
}

 
void loop() {
  tryConnectToWifi(_wifi_SSID,_wifi_password);  
  ArduinoOTA.handle();
  
  // Reading temperature or humidity takes about 250 milliseconds! 
  float h = dht.readHumidity();  
  float t = dht.readTemperature();
  
  delay(10000);
    
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");  
  Serial.println("");
}

bool tryConnectToWifi(char* ssid, char* password) {
  bool _isWiFiConnected = isWiFiConnected();
  if(_isWiFiConnected){
    return _isWiFiConnected;
  }
  int connectionDelay = 0;
  
  Serial.print("Connecting to ");
  Serial.println(ssid);  
  
  WiFiMulti.addAP(ssid, password);
  while(!isWiFiConnected()){
    if(connectionDelay > 4){
      break;
    }
    delay(500);
    Serial.print(".");
    connectionDelay++;
  }

  if(_isWiFiConnected){
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else{
    Serial.println("");
    Serial.println("WiFi NOT connected");  
  }
  
  return _isWiFiConnected;
}

bool isWiFiConnected(){
  _isWiFiConnected = WiFiMulti.run() == WL_CONNECTED;  
   return _isWiFiConnected;  
}
 
  

 

