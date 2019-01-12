#ifndef RF24SN_h
#define RF24SN_h

#include "RF24SN-Packet.h"
#include "RF24SN-MQTT-Wrapper.h"
#include "RF24SN-RF24-Wrapper.h"
#include "RF24SN-Log.h"

#include <cstdlib> 
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

void processPacket(Packet packet);
void processPublishPacket(Packet packet);
void processRequestPacket(Packet packet);

void setup(void);
int loop(void);
int main(int argc, char** argv);

#endif