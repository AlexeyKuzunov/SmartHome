#ifndef RF24SN_Packet_h
#define RF24SN_Packet_h

#include <stdint.h>
#include "RF24SN-Log.h"
#include <iostream> 


typedef struct{
  uint8_t packetType; 		// 0 reserved, 1=publish, 2=puback, 3=request, 4=response
  uint8_t SensorType;		// 0 reserved, 1=sensor, 2=switch
  uint8_t nodeId; 			//sensor node id, also used for return address calcualtion
  uint8_t sensorId; 		//sensor id
  float value; 				//sensor reading
} Packet;

static const uint8_t NO_PACKET = 0;
static const uint8_t PUBLISH_PACKET = 1;
static const uint8_t PUBACK_PACKET = 2;
static const uint8_t REQUEST_PACKET = 3;
static const uint8_t RESPONSE_PACKET = 4;

static const uint8_t SENSOR = 1;
static const uint8_t SWITCH = 2;


void printPacket(char block[128], Packet packet);
void printPacket(Packet packet);

#endif


