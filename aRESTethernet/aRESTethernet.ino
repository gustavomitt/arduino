/* 
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the Ethernet library (for example to be used with the Ethernet shield). 
  See the README file for more details.
 
  Written in 2014 by Marco Schwartz under a GPL license. 
*/

// Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <aREST.h>
#include <avr/wdt.h>
#include <Timer.h>

// Timer
Timer t;

// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x40 };

// IP address in case DHCP fails
IPAddress ip(192,168,2,230);

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

// Variables to be exposed to the API
int humidity = 0;  // variable to store the value coming from the humidity sensor

//Varibles of physical connections
int sensorPin = A5;    // select the input pin for the sensor
int activatePin = 7;    // select the pin to turn on the sensor
int pumpPin = 8;     // select the pin to turn on the pump

void setup(void)
{  
  // Start Serial
  Serial.begin(115200);
  
  // Init pin to activate humidity sensor
  pinMode(activatePin,OUTPUT);
  digitalWrite(activatePin,LOW);
  
  // Init pin to activate the pump
  pinMode(pumpPin,OUTPUT);
  digitalWrite(pumpPin,LOW);

  // Init variables and expose them to REST API
  rest.variable("humidity",&humidity);

  // Function to be exposed
  //rest.function("pumpOn",pumpOn);
  //rest.function("pumpOff",pumpOff);
  
  // Give name and ID to device
  rest.set_id("002");
  rest.set_name("garden_control2");

  // Start the Ethernet connection and the server
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Start watchdog
  wdt_enable(WDTO_4S);
  
  // Setup events
  t.every(30000, readHumiditySensor); // Read humidity sensor every 30 seconds
}

void readHumiditySensor()
{
  digitalWrite(activatePin,HIGH);
  delay(100);
  humidity = analogRead(sensorPin);// reading humidity sensor value
  digitalWrite(activatePin,LOW); //Turn off the humidity sensor
}


void loop() {  
  
  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  wdt_reset();
  t.update();  
}

// Custom function accessible by the API
int pumpOn() {
  digitalWrite(pumpPin,HIGH);
  return 1;
}
int pumpOff() {
  digitalWrite(pumpPin,LOW);
  return 0;
}
