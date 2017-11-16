#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  

   bool connectionResult = tryConnectToWifi("eGO-WiFi","1111111111111");
   if(!connectionResult){
    //ESP.restart();
    Serial.println("Wifi not connected !");
   }
   else {
      ArduinoOTA.setHostname("Ego-ESP8266-tempSensor");
      ArduinoOTA.begin();      
   } 
   
  dht.begin();
}

 
void loop() {
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
      bool isConnected = false;
    
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      delay(15000);
    
     if( WiFi.waitForConnectResult() == WL_CONNECTED){
      return true;
     }
     else{
      return false;
     }
    
  }


 
  

 

