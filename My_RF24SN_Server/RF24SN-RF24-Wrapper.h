#ifndef RF24SN_RF24_Wrapper_h
#define RF24SN_RF24_Wrapper_h

#include "RF24.h"
#include <stdint.h>
#include "RF24SN-Packet.h"
#include "RF24SN-Log.h"

void RF24Setup(void (*processPacket)(Packet));
void sendPacket(Packet packet);
void RF24Loop(void);

#endif



