#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <PubSubClient.h>


#define DHTPIN 2
#define DHTTYPE DHT22
#define GetSensorsDataInterval 6000

String orgId = "";
String deviceType = "";
String deviceId = "";
String authToken = "";


bool _isWiFiConnected = false;
char* _wifi_SSID = "";
char* _wifi_password = "";

char* temperature_topic = "iot-2/evt/temperature/fmt/json";
char* humidity_topic = "iot-2/evt/humidity/fmt/json";
String server = orgId + ".messaging.internetofthings.ibmcloud.com";

DHT dht(DHTPIN, DHTTYPE);

WiFiClient wifiClient;

void callback(char* topic, byte* payload, unsigned int payloadLength);
PubSubClient client((char*)server.c_str(), 1883, callback, wifiClient);

void setup() {
  Serial.begin(115200);
  delay(100);  
  bool connectionResult = tryConnectToWifi(_wifi_SSID, _wifi_password);
  if (connectionResult) {
    dht.begin();
  }
  
}


void loop() {
  tryConnectToWifi(_wifi_SSID, _wifi_password);
  
  delay(GetSensorsDataInterval);

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  float t = dht.readTemperature();

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

  connectToMqtt();
  String payload_temp = "{\"d\":{\"temperature\":";
  payload_temp += t;
  payload_temp += "}}";

  String payload_hum = "{\"d\":{\"humidity\":";;
  payload_hum += h;
  payload_hum += "}}";


  Serial.print("Sending : ");
  Serial.println(payload_temp);
  Serial.println(payload_hum);

  if (client.publish(temperature_topic, (char*) payload_temp.c_str())) {
    Serial.println("Publish temperature ok");
    _isLastSendDataFailed = false;
  } else {
    _isLastSendDataFailed = true;
    Serial.println("Publish temperature failed");
  }
  if (client.publish(humidity_topic, (char*) payload_hum.c_str())) {
    Serial.println("Publish humidity ok");
    _isLastSendDataFailed = false;
  } else {
    Serial.println("Publish humidity failed");
    _isLastSendDataFailed = true;
  }

}

bool tryConnectToWifi(char* ssid, char* password) {
  if (isWiFiConnected()) {
    return true;
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (!isWiFiConnected()) {
    delay(200);
    Serial.print(".");
  }
  if (isWiFiConnected()) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("WiFi NOT connected");
  }
  return isWiFiConnected();
}

bool isWiFiConnected() {
  if(_isLastSendDataFailed){
    _isWiFiConnected = (WiFi.waitForConnectResult() == WL_CONNECTED);
  }  
  return _isWiFiConnected;
}

void connectToMqtt() {
  if(client.connected()){
    return;
  }
  String clientName;

  clientName += "d:";
  clientName += orgId + ":";
  clientName += deviceType + ":";
  clientName += deviceId;

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);

  if (client.connect((char*) clientName.c_str(), "use-token-auth", (char*) authToken.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    Serial.println(temperature_topic);
    Serial.println(humidity_topic);
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic: ");
  Serial.println(topic);
}



