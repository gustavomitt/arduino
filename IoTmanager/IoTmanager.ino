/*
 IoT Manager mqtt device client https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
 Based on Basic MQTT example with Authentication
 PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  - connects to an MQTT server, providing userdescr and password
  - publishes config to the topic "/IoTmanager/config/deviceID/"
  - subscribes to the topic "/IoTmanager/hello" ("hello" messages from mobile device) 
  Tested with Arduino IDE 1.6.6 + ESP8266 Community Edition v 2.0.0-stable and PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  ESP8266 Community Edition v 2.0.0-stable have some HTTPS issues. Push notification temporary disabled.
  sketch version : 1.5
  IoT Manager    : any version
  toggle, range, small-badge widgets demo
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <Syslog.h>
#include "credentials.h"


// A UDP instance to let us send and receive packets over UDP
WiFiUDP udpClient;

// Create a new syslog instance with LOG_info facility
Syslog syslog(udpClient, SYSLOG_SERVER, SYSLOG_PORT, DEVICE_HOSTNAME, APP_NAME, LOG_INFO);

const char *ssid =  SSID;            // cannot be longer than 32 characters!
const char *pass =  PASS;       // WiFi password

String prefix   = PREFIX;     // global prefix for all topics - must be some as mobile device
String deviceID = DEVICEID;   // thing ID - unique device id in our project
const int led = 14; // Led indicativo de atividade

WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU, m11.cloudmqtt.com - USA )
String mqttServerName = MQTTSERVERNAME;            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = MQTTPORT;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  MQTTUSER;                              // from CloudMQTT account data
String mqttpass =  MQTTPASS;                              // from CloudMQTT account data
PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname



String val;
String ids = "";
int oldtime, newtime, pushtime;
int newValue;

const String stat1 = "{\"status\":\"1\"}";
const String stat0 = "{\"status\":\"0\"}";

const int nWidgets = 8;
String stat        [nWidgets];
String sTopic      [nWidgets];
String color       [nWidgets];
String style       [nWidgets];
String badge       [nWidgets];
String widget      [nWidgets];
String descr       [nWidgets];
String page        [nWidgets];
String thing_config[nWidgets];
String id          [nWidgets];
int    pin         [nWidgets];
int    defaultVal  [nWidgets];
bool   inverted    [nWidgets];


String setStatus ( String s ) {
  String stat = "{\"status\":\"" + s + "\"}";
  return stat;
}
String setStatus ( int s ) {
  String stat = "{\"status\":\"" + String(s) + "\"}";
  return stat;
}
void initVar() {
  id    [0] = "0";
  page  [0] = "QuartoCasal";
  descr [0] = "relay1";
  widget[0] = "toggle";
  pin[0] = 5;                                              // GPIO5 - toggle
  defaultVal[0] = 1;                                       // defaultVal status
  inverted[0] = true;
  sTopic[0]   = prefix + "/" + deviceID + "/relay1";
  color[0]   = "\"color\":\"black\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [1] = "1";
  page  [1] = "QuartoCasal";
  descr [1] = "relay2";
  widget[1] = "toggle";
  pin[1] = 4;                                              // GPIO4 - toggle
  defaultVal[1] = 1;                                       // defaultVal status
  inverted[1] = true;
  sTopic[1]   = prefix + "/" + deviceID + "/relay2";
  color[1]   = "\"color\":\"blue\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [2] = "2";
  page  [2] = "QuartoCasal";
  descr [2] = "relay3";
  widget[2] = "toggle";
  pin[2] = 0;                                              // GPIO0 - toggle
  defaultVal[2] = 1;                                       // defaultVal status
  inverted[2] = true;
  sTopic[2]   = prefix + "/" + deviceID + "/relay3";
  color[2]   = "\"color\":\"green\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [3] = "3";
  page  [3] = "QuartoCasal";
  descr [3] = "relay4";
  widget[3] = "toggle";
  pin[3] = 2;                                              // GPIO2 - toggle
  defaultVal[3] = 1;                                       // defaultVal status
  inverted[3] = true;
  sTopic[3]   = prefix + "/" + deviceID + "/relay4";
  color[3]   = "\"color\":\"orange\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [4] = "4";
  page  [4] = "QuartoCasal";
  descr [4] = "relay5";
  widget[4] = "toggle";
  pin[4] = 14;                                              // GPIO14 - toggle
  defaultVal[4] = 1;                                       // defaultVal status
  inverted[4] = true;
  sTopic[4]   = prefix + "/" + deviceID + "/relay5";
  color[4]   = "\"color\":\"red\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [5] = "5";
  page  [5] = "QuartoCasal";
  descr [5] = "relay6";
  widget[5] = "toggle";
  pin[5] = 12;                                              // GPIO12 - toggle
  defaultVal[5] = 1;                                       // defaultVal status
  inverted[5] = true;
  sTopic[5]   = prefix + "/" + deviceID + "/relay6";
  color[5]   = "\"color\":\"white\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [6] = "6";
  page  [6] = "QuartoCasal";
  descr [6] = "relay7";
  widget[6] = "toggle";
  pin[6] = 13;                                              // GPIO13 - toggle
  defaultVal[6] = 1;                                       // defaultVal status
  inverted[6] = true;
  sTopic[6]   = prefix + "/" + deviceID + "/relay7";
  color[6]   = "\"color\":\"yellow\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [7] = "7";
  page  [7] = "QuartoCasal";
  descr [7] = "relay8";
  widget[7] = "toggle";
  pin[7] = 15;                                              // GPIO15 - toggle
  defaultVal[7] = 1;                                       // defaultVal status
  inverted[7] = true;
  sTopic[7]   = prefix + "/" + deviceID + "/relay8";
  color[7]   = "\"color\":\"black\"";                       // black, blue, green, orange, red, white, yellow (off - grey)
 

  for (int i = 0; i < nWidgets; i++) {
    if (inverted[i]) {
      if (defaultVal[i]>0) {
         stat[i] = setStatus(0);
      } else {
         stat[i] = setStatus(1);
      }
    } else {
       stat[i] = setStatus(defaultVal[i]);
    }
  }      

  thing_config[0] = "{\"id\":\"" + id[0] + "\",\"page\":\"" + page[0]+"\",\"descr\":\"" + descr[0] + "\",\"widget\":\"" + widget[0] + "\",\"topic\":\"" + sTopic[0] + "\"," + color[0] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[1] = "{\"id\":\"" + id[1] + "\",\"page\":\"" + page[1]+"\",\"descr\":\"" + descr[1] + "\",\"widget\":\"" + widget[1] + "\",\"topic\":\"" + sTopic[1] + "\"," + color[1] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[2] = "{\"id\":\"" + id[2] + "\",\"page\":\"" + page[2]+"\",\"descr\":\"" + descr[2] + "\",\"widget\":\"" + widget[2] + "\",\"topic\":\"" + sTopic[2] + "\"," + color[2] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[3] = "{\"id\":\"" + id[3] + "\",\"page\":\"" + page[3]+"\",\"descr\":\"" + descr[3] + "\",\"widget\":\"" + widget[3] + "\",\"topic\":\"" + sTopic[3] + "\"," + color[3] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[4] = "{\"id\":\"" + id[4] + "\",\"page\":\"" + page[4]+"\",\"descr\":\"" + descr[4] + "\",\"widget\":\"" + widget[4] + "\",\"topic\":\"" + sTopic[4] + "\"," + color[4] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[5] = "{\"id\":\"" + id[5] + "\",\"page\":\"" + page[5]+"\",\"descr\":\"" + descr[5] + "\",\"widget\":\"" + widget[5] + "\",\"topic\":\"" + sTopic[5] + "\"," + color[5] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[6] = "{\"id\":\"" + id[6] + "\",\"page\":\"" + page[6]+"\",\"descr\":\"" + descr[6] + "\",\"widget\":\"" + widget[6] + "\",\"topic\":\"" + sTopic[6] + "\"," + color[6] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[7] = "{\"id\":\"" + id[7] + "\",\"page\":\"" + page[7]+"\",\"descr\":\"" + descr[7] + "\",\"widget\":\"" + widget[7] + "\",\"topic\":\"" + sTopic[7] + "\"," + color[7] + "}";   // GPIO switched On/Off by mobile widget toggle
}
// send confirmation
void pubStatus(String t, String payload) {  
    if (client.publish(t + "/status", payload)) { 
       Serial.println("Publish new status for " + t + ", value: " + payload);
    } else {
       Serial.println("Publish new status for " + t + " FAIL!");
    }
}
void pubConfig() {
  bool success;
  success = client.publish(MQTT::Publish(prefix, deviceID).set_qos(1));
  if (success) {
      delay(500);
      for (int i = 0; i < nWidgets; i = i + 1) {
        success = client.publish(MQTT::Publish(prefix + "/" + deviceID + "/config", thing_config[i]).set_qos(1));
        if (success) {
          Serial.println("Publish config: Success (" + thing_config[i] + ")");
        } else {
          Serial.println("Publish config FAIL! ("    + thing_config[i] + ")");
        }
        delay(150);
      }      
  }
  if (success) {
     Serial.println("Publish config: Success");
  } else {
     Serial.println("Publish config: FAIL");
  }
  for (int i = 0; i < nWidgets; i = i + 1) {
      pubStatus(sTopic[i], stat[i]);
      delay(150);
  }      
}


void callback(const MQTT::Publish& sub) {
  Serial.print("Get data from subscribed topic ");
  Serial.print(sub.topic());
  Serial.print(" => ");
  Serial.println(sub.payload_string());

  if (sub.topic() == sTopic[0] + "/control") {
    String acao;
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[0] = stat0;
       acao = "desligado";
    } else {
       newValue = 0;
       stat[0] = stat1;
       acao = "ligado";
    }
    digitalWrite(pin[0],newValue);
    pubStatus(sTopic[0], stat[0]);
    String mensagem = descr[0] + " " + acao + " por comando remoto";
    syslog.log(LOG_INFO, mensagem);
 } else if (sub.topic() == sTopic[1] + "/control") {
    String acao;
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[1] = stat0;
       acao = "desligado";
    } else {
       newValue = 0; // inverted
       stat[1] = stat1;
    }
    digitalWrite(pin[1],newValue);
    pubStatus(sTopic[1], stat[1]);
    String mensagem = descr[1] + " " + acao + " por comando remoto";
    syslog.log(LOG_INFO, mensagem);
 } else if (sub.topic() == sTopic[2] + "/control") {
    String acao;
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[2] = stat0;
       acao = "desligado";
    } else {
       newValue = 0; // inverted
       stat[2] = stat1;
       acao = "ligado";
    }
    digitalWrite(pin[2],newValue);
    pubStatus(sTopic[2], stat[2]);
    String mensagem = descr[2] + " " + acao + " por comando remoto";
    syslog.log(LOG_INFO, mensagem);
 } else if (sub.topic() == sTopic[3] + "/control") {
    String acao;
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[3] = stat0;
       acao = "desligado";
    } else {
       newValue = 0; // inverted
       stat[3] = stat1;
       acao = "ligado";
    }
    digitalWrite(pin[3],newValue);
    pubStatus(sTopic[3], stat[3]);
    String mensagem = descr[3] + " " + acao + " por comando remoto";
    syslog.log(LOG_INFO, mensagem);
 } else if (sub.topic() == sTopic[4] + "/control") {
    String acao;
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[4] = stat0;
       acao = "desligado";
    } else {
       newValue = 0; // inverted
       stat[4] = stat1;
       acao = "ligado";
    }
    digitalWrite(pin[4],newValue);
    pubStatus(sTopic[4], stat[4]);
    String mensagem = descr[4] + " " + acao + " por comando remoto";
    syslog.log(LOG_INFO, mensagem);
 } else if (sub.topic() == sTopic[5] + "/control") {
    String acao;
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[5] = stat0;
       acao = "desligado";
    } else {
       newValue = 0; // inverted
       stat[5] = stat1;
       acao = "ligado";
    }
    digitalWrite(pin[5],newValue);
    pubStatus(sTopic[5], stat[5]);
    String mensagem = descr[5] + " " + acao + " por comando remoto";
    syslog.log(LOG_INFO, mensagem);
 } else if (sub.topic() == sTopic[6] + "/control") {
    String acao;
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[6] = stat0;
       acao = "desligado";
    } else {
       newValue = 0; // inverted
       stat[6] = stat1;
       acao = "ligado";
    }
    digitalWrite(pin[6],newValue);
    pubStatus(sTopic[6], stat[6]);
    String mensagem = descr[6] + " " + acao + " por comando remoto";
    syslog.log(LOG_INFO, mensagem);
 } else if (sub.topic() == sTopic[7] + "/control") {
    String acao;
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[7] = stat0;
       acao = "desligado";
    } else {
       newValue = 0; // inverted
       stat[7] = stat1;
       acao = "ligado";
    }
    digitalWrite(pin[7],newValue);
    pubStatus(sTopic[7], stat[7]);
    String mensagem = descr[7] + " " + acao + " por comando remoto";
    syslog.log(LOG_INFO, mensagem);
 }
}

void setup() {
  initVar();
  WiFi.mode(WIFI_STA);
  pinMode(pin[0], OUTPUT);
  digitalWrite(pin[0],defaultVal[0]);
  pinMode(pin[1], OUTPUT);
  digitalWrite(pin[1],defaultVal[1]);
  pinMode(pin[2], OUTPUT);
  digitalWrite(pin[2],defaultVal[2]);
  pinMode(pin[3], OUTPUT);
  digitalWrite(pin[3],defaultVal[3]);
  pinMode(pin[4], OUTPUT);
  digitalWrite(pin[4],defaultVal[4]);
  pinMode(pin[5], OUTPUT);
  digitalWrite(pin[5],defaultVal[5]);
  pinMode(pin[6], OUTPUT);
  digitalWrite(pin[6],defaultVal[6]);
  pinMode(pin[7], OUTPUT);
  digitalWrite(pin[7],defaultVal[7]);
  pinMode(led, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Free heap = ");
  Serial.println(ESP.getFreeHeap());
  // Set the watchdog timer
  ESP.wdtEnable(30000);
}

void loop() {
  digitalWrite(led, LOW);  // Turn the on-board LED on
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting via WiFi to ");
    Serial.print(ssid);
    Serial.println("...");

    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      return;
    }

    Serial.println("");
    Serial.println("WiFi connect: Success");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    syslog.log(LOG_INFO, "Wifi conectado.");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server ...");
      syslog.log(LOG_INFO, "Conectando ao servidor MQTT.");
      bool success;
      if (mqttuser.length() > 0) {
        success = client.connect( MQTT::Connect( deviceID ).set_auth(mqttuser, mqttpass) );
      } else {
        success = client.connect( deviceID );
      }
      if (success) {
        client.set_callback(callback);
        Serial.println("Connect to MQTT server: Success");
        syslog.log(LOG_INFO, "Conectado ao servidor MQTT.");
        pubConfig();
        client.subscribe(prefix);                 // for receiving HELLO messages
        client.subscribe(prefix + "/ids");        // for receiving IDS  messages
        client.subscribe(sTopic[0] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[1] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[2] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[3] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[4] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[5] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[6] + "/control"); // for receiving GPIO messages
        client.subscribe(sTopic[7] + "/control"); // for receiving GPIO messages
        Serial.println("Subscribe: Success");
      } else {
        Serial.println("Connect to MQTT server: FAIL");   
        syslog.log(LOG_ERR, "Conexao ao servidor MQTT falhou.");
        delay(1000);
      }
    }

    if (client.connected()) {
      client.loop();
    }
  }
  // Reset watchdog
  ESP.wdtFeed();
  delay(1000);
  digitalWrite(led, HIGH);  // Turn the LED off
  delay(1000);
}
