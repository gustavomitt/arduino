# modelo do credentials.h:
```
// Syslog server connection info
#define SYSLOG_SERVER "192.168.1.xxxx"
#define SYSLOG_PORT 514

// This device info
#define DEVICE_HOSTNAME "xxxx"
#define APP_NAME "xxxx"


// WiFi
#define SSID "xxxx"            // cannot be longer than 32 characters!
#define PASS  "xxxx"       // WiFi password

//Formatacao das mensagens
#define PREFIX "/xxxx"     // global prefix for all topics - must be some as mobile device
#define DEVICEID "xxxx"   // thing ID - unique device id in our project

//MQTT server
#define MQTTSERVERNAME = "xxx.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
#define MQTTPORT = 1234;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
#define MQTTUSER =  "xxxx";                              // from CloudMQTT account data
#define MQTTPASS =  "xxxx";                              // from CloudMQTT account data
```
