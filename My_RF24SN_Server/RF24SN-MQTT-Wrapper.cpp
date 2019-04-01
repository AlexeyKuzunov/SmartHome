#include "RF24SN-MQTT-Wrapper.h"

using namespace std;

std::map<string, float> messageStore;

struct mosquitto *mqtt;

void onMessage(struct mosquitto *m, void *udata, const struct mosquitto_message *msg)
{
    if (msg == NULL) { return; }
    float value;
    const char* pch = ((char *)msg->payload);
    stringstream(pch) >> value;
    string topic = msg->topic;
    messageStore[topic] = value;
//    cout << "received MQTT message, topic: " <<  topic << " value: " << value << endl;
}

void mqttSetup(void)
{
	mqtt = mosquitto_new(NULL, true, NULL);
	if(!mqtt)
	{
	    cerr << "Can`t initialize Mozquitto library\n" << endl;
	}
	int ret = mosquitto_username_pw_set(mqtt, "alex", "216!239!");
	if(ret)
	{
	    cerr <<  "Can`t login  Mozquitto server\n" << endl;
	}
	ret = mosquitto_connect(mqtt, "localhost", 1883, 60);
	if(ret)
	{
	    cerr << "Can`t connect Mozquitto server\n" << endl;
	}
	
	mosquitto_message_callback_set(mqtt, onMessage);
	mosquitto_subscribe(mqtt, NULL, "RF24SN/out/+/+", 0);
}


void mqttPublish(int nodeId, int sensorId, float value)
{
    string topic = mqttTopic(nodeId, sensorId);
	
	std::stringbuf payload;      
  	std::ostream ps (&payload);  
	ps << value;
	
	int ret = mosquitto_publish(mqtt, NULL, topic.c_str(), payload.str().size(), payload.str().c_str(), 0, false);
	if(ret)
	{
		mqttSetup();
	    cerr << "Can`t publish Mozquitto server\n" << endl;
	}
	
}

float mqttRequest(int nodeId, int sensorId)
{
    string topic = mqttTopic(nodeId, sensorId);
	return messageStore[topic];
}

/**
* Формирует сообщение для Mosquitto вида "RF24SN/in/1/1"
*
* nodeId - номер модуля
*
*sensorId - номер сенсора
*
*/

string mqttTopic(int nodeId, int sensorId){
	std::stringbuf topic;      
  	std::ostream ts (&topic);  

	//при необходимости разграничения приема и передачи
	//тогда в значения функции добавить bool in
	//ts << "RF24SN/" << (in?"in":"out") << "/" << nodeId << "/" << sensorId;

	ts << "RF24SN/" << nodeId << "/" << sensorId;
	//cout << "ts: " << topic.str() << "\n";
	return topic.str();
}


void mqttLoop(void){
    mosquitto_loop(	mqtt, 50, 42 /* the answer to life universe and everything */ );
}
	

