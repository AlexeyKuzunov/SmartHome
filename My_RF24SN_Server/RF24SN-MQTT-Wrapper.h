#ifndef RF24SN_MQTT_Wrapper_h
#define RF24SN_MQTT_Wrapper_h

#include <string>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <mosquitto.h>
#include "RF24SN-Log.h"

void onMessage(struct mosquitto *m, void *udata, const struct mosquitto_message *msg);
void mqttSetup(void);
void mqttPublish(int nodeId, int sensorId, float value);
void mqttPublishSw(int nodeId, int sensorId, float value);

float mqttRequest(int nodeId, int sensorId);
std::string mqttTopic(int nodeId, int sensorId, bool in);
void mqttLoop(void);

#endif