
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>
#include <Servo.h>

// Servo variables 
// Digital pins 2-9 used for servo control
// Digital pins 10 - 13 used by Ethernet shield

Servo myservo;  // create Servo object to control a servo
Servo myservo2; // 48 Servo objects can be created on the Mega; we are only using pins 22-53 due to the Ethernet shield

int pos = 0;    // variable to store the servo position

//Ethernet variables
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
EthernetClient ethClient;

//Modbus variables
//ModbusTCPClient modbusTCPClient(ethClient);
//IPAddress server(192, 168, 1, 10); // update with the IP Address of your Modbus server

void setup() {
  Serial.begin(9600);
  Serial.println("startup");
  myservo.attach(22);   // attaches the servo on pin 22 to the Servo object
  myservo2.attach(23);
 

  // start the Ethernet connection:
  Ethernet.init(10);  // Most Arduino shields
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  switch (Ethernet.maintain()) {
    case 1:
      //renewed fail
      Serial.println("Error: renewed fail");
      break;

    case 2:
      //renewed success
      Serial.println("Renewed success");
      //print your local IP address:
      Serial.print("My IP address: ");
      Serial.println(Ethernet.localIP());
      break;

    case 3:
      //rebind fail
      Serial.println("Error: rebind fail");
      break;

    case 4:
      //rebind success
      Serial.println("Rebind success");
      //print your local IP address:
      Serial.print("My IP address: ");
      Serial.println(Ethernet.localIP());
      break;

    default:
      //nothing happened
      break;
  }
  /*
  if (!modbusTCPClient.connected()) {
  // client not connected, start the Modbus TCP client
    Serial.println("Attempting to connect to Modbus TCP server");
    
    if (!modbusTCPClient.begin(server, 502)) {
      Serial.println("Modbus TCP Client failed to connect!");
    } else {
      Serial.println("Modbus TCP Client connected");
    }
  } else {
  // client connected
  // write the value of 0x01, to the coil at address 0x00
    if (!modbusTCPClient.coilWrite(0x00, 0x01)) {
      Serial.print("Failed to write coil! ");
      Serial.println(modbusTCPClient.lastError());
    }
    // wait for 1 second
    delay(1000);
    // write the value of 0x00, to the coil at address 0x00
    if (!modbusTCPClient.coilWrite(0x00, 0x00)) {
      Serial.print("Failed to write coil! ");
      Serial.println(modbusTCPClient.lastError());
    }
    // wait for 1 second
    delay(1000);
  }
*/
Serial.println("servo 1 open");
myservo.write(60);              // tell servo to go to position in variable 'pos'
Serial.println("servo 2 open");
myservo2.write(60);              // tell servo to go to position in variable 'pos'
delay(500);
Serial.println("servo 1 close");
myservo.write(90);              // tell servo to go to position in variable 'pos'
Serial.println("servo 2 close");
myservo2.write(45);              // tell servo to go to position in variable 'pos'
delay(500);
}
