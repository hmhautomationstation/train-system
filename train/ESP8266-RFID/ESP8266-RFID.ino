//include needed libraries
#include <MFRC522.h>  // Library for Mifare RC522 Devices
#include <SPI.h>
#include <ESP8266WiFi.h>


// WiFi settings
const char *ssid = "WIFI_SSID";             // Replace with your WiFi name
const char *password = "WIFI_PASSWORD";   // Replace with your WiFi password
WiFiClient espClient;

//init MQTT
const char *mqtt_broker = "broker.emqx.io";  // EMQX broker endpoint
const char *mqtt_topic = "emqx/esp8266/led";  // MQTT topic
const char *mqtt_username = "emqx";  // MQTT username for authentication
const char *mqtt_password = "public";  // MQTT password for authentication
const int mqtt_port = 1883;  // MQTT port (TCP)
PubSubClient mqtt_client(espClient);

//init RFID
//4 bytes tag serial number, the first 5 bytes for the checksum byte
uint8_t successRead;    // Variable integer to keep if we have Successful Read from Reader
byte readCard[4];   // Stores scanned ID read from RFID Module

void setup() {
  //Start by connecting to wifi
  Serial.begin(115200);
  // start the SPI library:
  SPI.begin();
  WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to the WiFi network");

  //start MQTT
  while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            // Publish message upon successful connection
            mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP8266 ^^");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }

  //start RFID
    mfrc522.PCD_Init();    // Initialize MFRC522 Hardware

}

void loop() {
  //check for new tag
    do {
    successRead = getID();  // sets successRead to 1 when we get read from reader otherwise 0
    // When device is in use if wipe button pressed for 10 seconds initialize Master Card wiping
  }
  while (!successRead);
  //when found, send to appropriate topic
mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP8266 ^^");
}

uint8_t getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println(F("Scanned PICC's UID:"));
  for ( uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}