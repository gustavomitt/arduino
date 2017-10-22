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

// Parametros default de umidade
const float umidadeMax = 600.0;
const float umidadeMin = 700.0;

boolean publicar;

float x; // media dos valores lidos


// Parametros para a leitura do sensor
const int intervaloDeLeitura = 10;  // intervalo entre as leituras
const int numeroDeLeituras = 10;     // numero de leituras cuja media sera enviada para o mqtt

WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU, m11.cloudmqtt.com - USA )
String mqttServerName = MQTTSERVERNAME;            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = MQTTPORT;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  MQTTUSER;                              // from CloudMQTT account data
String mqttpass =  MQTTPASS;                              // from CloudMQTT account data
PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname

String prefix   = PREFIX;     // global prefix for all topics - must be some as mobile device
String deviceID = DEVICEID;   // thing ID - unique device id in our project


String val;
String ids = "";
int oldtime, newtime, pushtime;
int newValue;

int rodada;

const String stat1 = "{\"status\":\"1\"}";
const String stat0 = "{\"status\":\"0\"}";

const int nWidgets = 2;
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

int    valores     [10];
int    activatePin = 4;            // GPIO4 - pino para ligar o sensor

int estadoAtual = LOW;  //Estado atual do pino de controle da bomba


// Push notifications
const char* host = "onesignal.com";
WiFiClientSecure httpClient;
const int httpsPort = 443;
String url = "/api/v1/notifications";

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
  page  [0] = "Vaso";
  descr [0] = "Bomba";
  widget[0] = "toggle";
  pin[0] = 5;                                              // GPIO5 - toggle
  defaultVal[0] = 0;                                       // defaultVal status
  inverted[0] = false;
  sTopic[0]   = prefix + "/" + deviceID + "/bomba";
  color[0]   = "\"color\":\"black\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [1] = "1";
  page  [1] = "Vaso";
  descr [1] = "Humidade";
  widget[1] = "small-badge";
  pin   [1] = A0;                                          // ADC
  sTopic[1] = prefix + "/" + deviceID + "/Humidade";
  badge [1] = "\"badge\":\"badge-calm\"";                  // see http://ionicframework.com/docs/components/#colors
  style [1]   = "\"style\":\"font-size:150%;\"";


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
  thing_config[1] = "{\"id\":\"" + id[1] + "\",\"page\":\"" + page[1]+"\",\"descr\":\"" + descr[1] + "\",\"widget\":\"" + widget[1] + "\",\"topic\":\"" + sTopic[1] + "\"," + badge[1] + "," + style[1] + "}";  // Humidade

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
    if (sub.payload_string() == "0") {
       newValue = 0;
       stat[0] = stat0;
       digitalWrite(pin[0],newValue);
       pubStatus(sTopic[0], stat[0]);
    } else {
       newValue = 1;
       stat[0] = stat1;
       digitalWrite(pin[0],newValue);
       pubStatus(sTopic[0], stat[0]);
       //newtime = millis();
       //oldtime = newtime;
       //while (newtime - oldtime < 10000) { // 10 sec
       //  delay(100);
       //  newtime =  millis();
       //}
       //newValue = 0;
       //stat[0] = stat0;
       //digitalWrite(pin[0],newValue);
       //pubStatus(sTopic[0], stat[0]);
    }
  } else if (sub.topic() == sTopic[1] + "/control") {
     // ADC : nothing, display only
  }
}

void setup() {
  initVar();
  WiFi.mode(WIFI_STA);
  pinMode(pin[0], OUTPUT);
  digitalWrite(pin[0],defaultVal[0]);
  stat[1] = setStatus(analogRead(pin[1]));
  // Init pin to activate humidity sensor
  pinMode(activatePin,OUTPUT);
  digitalWrite(activatePin,LOW);

  syslog.logMask(LOG_MASK(LOG_INFO) | LOG_MASK(LOG_ERR));
  rodada = 0;
  x = 0.0;


  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("MQTT client started.");
  Serial.print("Free heap = ");
  Serial.println(ESP.getFreeHeap());
  // Set the watchdog timer
  ESP.wdtEnable(30000);
}

void loop() {
  newtime = millis();
  syslog.logf(LOG_DEBUG, "Begin loop at %lu milliseconds", newtime);
  if (newtime - oldtime > intervaloDeLeitura * 1000) {
    syslog.log(LOG_INFO, "Iniciando nova leitura do sensor de umidade");
    String mensagem = "Rodada:" + String(rodada);
    syslog.log(LOG_INFO, mensagem);
    digitalWrite(activatePin,HIGH); // Liga o sensor
    delay(100);
    valores[rodada] = analogRead(pin[1]);
    mensagem = "Valor lido:" + String(valores[rodada]);
    syslog.log(LOG_INFO, mensagem);
    digitalWrite(activatePin,LOW); // desliga o sensor
    if (rodada >= numeroDeLeituras) {
      x = 0.0;
      for(int i = 0; i < numeroDeLeituras; i++){
        x = x + float(valores[i]);
      }
      x = x / numeroDeLeituras;
      val = "{\"status\":\"" + String(x)+ "\"}";
      syslog.log(LOG_INFO, val);
      rodada = 0;
      publicar = true;
    } else {
      rodada = rodada +1;
    }
    oldtime = newtime;
  }

  if( (estadoAtual == LOW) && ( x > umidadeMin) ){
    syslog.log(LOG_INFO, "Ligando a bomba");
    stat[0] = stat1;
    digitalWrite(pin[0],HIGH);
    pubStatus(sTopic[0], stat[0]);
    estadoAtual == HIGH;
  }

  if( (estadoAtual == HIGH) && ( x < umidadeMax) ){
    syslog.log(LOG_INFO, "Desligando a bomba");
    stat[0] = stat0;
    digitalWrite(pin[0],LOW);
    pubStatus(sTopic[0], stat[0]);
    estadoAtual == LOW;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting via WiFi to ");
    Serial.print(SSID);
    Serial.println("...");

    WiFi.begin(SSID, PASS);

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
        // 1 - display only, no control
        Serial.println("Subscribe: Success");
      } else {
        Serial.println("Connect to MQTT server: FAIL");
        syslog.log(LOG_ERR, "Conexao ao servidor MQTT falhou.");
        delay(1000);
      }
    }

    if (client.connected()) {
      if (rodada == 0 && publicar) {
        String mensagem = "Publicando mensagem:" + val;
        syslog.log(LOG_INFO, mensagem);
        if(client.publish(sTopic[1] + "/status", val )){
          syslog.log(LOG_INFO, "Mensagem publicada com sucesso");
        }else{
          syslog.log(LOG_INFO, "Falha na publicacao da mensagem");
        }
        publicar = false;
      }
      client.loop();
    }
  }
  // Reset watchdog
    ESP.wdtFeed();
}
