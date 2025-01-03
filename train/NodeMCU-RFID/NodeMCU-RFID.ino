#include <AddicoreRFID.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ArduinoMqttClient.h>

//wifi constants
#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

//MQTT constants
const char* host = "djxmmx.net";
const uint16_t port = 17;
const char brokerAddr[] = "192.168.178.20";     // Use the real broker ip address
const char topic[] = "/home/room/temperature";
const long intTimer = 5000;               // Timer interval for publishing the message
int currentTemp = 25;                     // Simulated temperature value in °C
unsigned long prevTimer = 0;              // Variable used to keep track of the previous timer value
unsigned long actualTimer = 0;            // Variable used to keep track of the current timer value

//RFID constants
#define	uchar	unsigned char
#define	uint	unsigned int
//4 bytes tag serial number, the first 5 bytes for the checksum byte
uchar serNumA[5];
uchar fifobytes;
uchar fifoValue;
AddicoreRFID myRFID; // create AddicoreRFID object to control the RFID module

/////////////////////////////////////////////////////////////////////
//set the pins
/////////////////////////////////////////////////////////////////////
const int chipSelectPin = 10;
const int NRSTPD = 5;

//Maximum length of the array
#define MAX_LEN 16

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
 // start the SPI library:
  SPI.begin();
  
  pinMode(chipSelectPin,OUTPUT);              // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin 
    digitalWrite(chipSelectPin, LOW);         // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);                     // Set digital pin 10 , Not Reset and Power-down
    digitalWrite(NRSTPD, HIGH);

  myRFID.AddicoreRFID_Init();  

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("[MQTT]: Connecting to MQTT broker: ");
  Serial.println(brokerAddr);

MqttClient mqttClient(WiFi);
  // Connect to MQTT broker
  if (!mqttClient.connect(brokerAddr, 1883)) {
    Serial.print("[MQTT]: MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    // The connection to the MQTT broker failed, Arduino is halted.
    while (1);
  }

  Serial.println("[MQTT]: Connection established");
  Serial.println();
}

void loop() {
    	uchar i, tmp, checksum1;
	uchar status;
        uchar str[MAX_LEN];
        uchar RC_size;
        uchar blockAddr;	//Selection operation block address 0 to 63
        String mynum = "";

        str[1] = 0x4400;
	//Find tags, return tag type
	status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);	
	if (status == MI_OK)
	{
            Serial.println("RFID tag detected");
    	    Serial.print(str[0],BIN);
            Serial.print(" , ");
    	    Serial.print(str[1],BIN);
            Serial.println(" ");
	}

	//Anti-collision, return tag serial number 4 bytes
	status = myRFID.AddicoreRFID_Anticoll(str);
	if (status == MI_OK)
	{
            checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
            Serial.println("The tag's number is  : ");
    	    //Serial.print(2);
    	    Serial.print(str[0]);
            Serial.print(" , ");
    	    Serial.print(str[1],BIN);
            Serial.print(" , ");
    	    Serial.print(str[2],BIN);
            Serial.print(" , ");
    	    Serial.print(str[3],BIN);
            Serial.print(" , ");
    	    Serial.print(str[4],BIN);
            Serial.print(" , ");
            Serial.println(checksum1,BIN);
     
  
               
            } 
            delay(500);
	}

        myRFID.AddicoreRFID_Halt();		   //Command tag into hibernation    
 mqttClient.poll();

  actualTimer = millis();

  // Check if the timer is expired
  if (actualTimer - prevTimer >= intTimer) {
    // save the last timestamp a message was sent
    prevTimer = actualTimer;

    // Post some debugging information on serial port
    Serial.print("[MQTT]: Sending message to topic: ");
    Serial.println(topic);
    Serial.println(currentTemp);

    // Publish message
    mqttClient.beginMessage(topic);
    mqttClient.print(currentTemp);
    mqttClient.endMessage();

    Serial.println();
  }

}
