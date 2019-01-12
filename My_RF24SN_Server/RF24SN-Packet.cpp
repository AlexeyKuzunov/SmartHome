//#include <iostream>
#include "RF24SN-Packet.h"

using namespace std;

void printPacket(char block[128], Packet packet)
{
	cout << getTime() << block <<" packetType: " << ((int)packet.packetType) << " nodeId: " << ((int)packet.nodeId) << " sensorId: " << ((int)packet.sensorId)<< " value: " << packet.value << endl;
} 

void printPacket(Packet packet)
{
	cout << getTime() <<" packetType: " << ((int)packet.packetType) << " nodeId: " << ((int)packet.nodeId) << " sensorId: " << ((int)packet.sensorId)<< " value: " << packet.value << endl;
} 
