#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ModbusIP_ESP8266.h>
#include <Servo.h>

//modbus constants
const int REG = 528;               // Modbus Hreg Offset
IPAddress remote(192, 168, 30, 13);  // Address of Modbus Slave device
ModbusIP mb;  //ModbusIP object
uint16_t res = 0; // 0 = main line, 1 = spur

//wifi constants
#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

//servo
Servo myservo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // We start by connecting to a WiFi network
 // start the SPI library:
  SPI.begin();

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
  
  //initialize modbus as client
  mb.client();

  //initialize servo
  myservo.attach(2);  // attaches the servo on GIO2 to the servo object

}

void loop() {
  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
    mb.readHreg(remote, REG, &res);  // Initiate Read Coil from Modbus Slave
  } else {
    mb.connect(remote);           // Try to connect if no connection
  }
  mb.task();                      // Common local Modbus task
  delay(100);                     // Pulling interval
  if (!res) {                     // if position = 0, set to position 5
    myservo.write(5);
  } else {                        // else, set to position 85
    myservo.write(85);
  }
}