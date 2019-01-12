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
    string topic = mqttTopic(nodeId, sensorId, true);
	
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

void mqttPublishSw(int nodeId, int sensorId, float value)
{
    for(int i=0;i=1;i++) {
	string topic = mqttTopic(nodeId, sensorId, i);
	
	std::stringbuf payload;      
  	std::ostream ps (&payload);  
	ps << value;
	
	int ret = mosquitto_publish(mqtt, NULL, topic.c_str(), payload.str().size(), payload.str().c_str(), 0, false);
	if(ret)
		{
	  		cerr << "Can`t publish Mozquitto server\n" << endl;
		}

	}
}


float mqttRequest(int nodeId, int sensorId)
{
    string topic = mqttTopic(nodeId, sensorId, false);
	return messageStore[topic];
}


string mqttTopic(int nodeId, int sensorId, bool in)
{
	
	std::stringbuf topic;      
  	std::ostream ts (&topic);  
	ts << "RF24SN/" << (in?"in":"out") << "/" << nodeId << "/" << sensorId;
	//cout << "ts: " << topic.str() << "\n";
	return topic.str();
	
}


void mqttLoop(void)
{

    mosquitto_loop(	mqtt, 50, 42 /* the answer to life universe and everything */ );

}
	

