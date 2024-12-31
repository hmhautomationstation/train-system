// TODO: add motor driving capabilities

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//wifi constants
const char* ssid = "YOUR-WIFI";
const char* password = "YOUR-WIFI-PSK";

//MQTT constants
const char* mqtt_server = "BROKER";
const uint16_t mqtt_server_port = 1883; 
const char* mqttUser = "user";
const char* mqttPassword = "pass";
const char* mqttTopicIn = "esp-8266-in";
const char* mqttTopicOut = "esp-8266-out";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
 // start the SPI library:
  SPI.begin();
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  setup_wifi();
  mqttClient.setServer(mqtt_server, mqtt_server_port);
  mqttClient.setCallback(callback);
}

void loop() {
 
 if (!mqttClient.connected()) {
23|     connect();
24|   }
25|   mqttClient.loop();

}

void connect() {
   while (!mqttClient.connected()) {
     Serial.print("Attempting MQTT connection...");
     String mqttClientId = "";
     if (mqttClient.connect(mqttClientId.c_str(), mqttUser, mqttPassword)) {
       Serial.println("connected");
       mqttClient.subscribe(mqttTopicIn);
     } else {
       Serial.print("failed, rc=");
       Serial.print(mqttClient.state());
       Serial.println(" will try again in 5 seconds");
       delay(5000);
     }
   }
 }

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: '");
  Serial.print(topic);
  Serial.print("' with payload: ");
  for (unsigned int i = 0; i < length; i++) {
   Serial.print((char)payload[i]);
  }
  Serial.println();
  String myCurrentTime = timeClient.getFormattedTime();
  mqttClient.publish(mqttTopicOut,("Time: " + myCurrentTime).c_str());
}
