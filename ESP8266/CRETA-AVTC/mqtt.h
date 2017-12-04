#ifndef MQTT_H
#define MQTT_H

/*************************************************/
/*                  FUCTION PROTOTYPE            */
/*************************************************/
void callback(char* topic, byte* payload, unsigned int length);
void mqttCreateTopic(void);
void mqttSubscribe(void);
int mqttConnect (void);
int mqttConnected (void);
void mqttPublish (String jsonOut);
void mqttLoop (void);
String Get_macID (void);
void mqttPubTest (void);

#endif /* MQTT_H */
