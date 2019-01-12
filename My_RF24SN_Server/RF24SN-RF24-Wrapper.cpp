#include "RF24SN-RF24-Wrapper.h"

//#include <iostream>
//#include "RF24.h"

//TODO load from program arguments
RF24 radio(2,"/dev/spidev0.0");

uint64_t base_pipe = 0xF0F0F0F000LL;      // address of the pipe to the base.

void (*_processPacket)(Packet); //Указатель на функцию 

void RF24Setup( void (*processPacket)(Packet) )
{
//  char log[256];
  radio.begin();
  radio.setPayloadSize(sizeof(Packet));
  //radio.setChannel(4);
  //radio.setDataRate(RF24_250KBPS);
  //radio.setRetries(5, 15);

  radio.openReadingPipe(1,base_pipe);
  radio.setAutoAck(1, false); //
//  radio.setAutoAck(1, true);
  radio.startListening();

//TODO prety print based on --verbose argument
//  printf("\n\rRF24SensorHub!\n");
//  radio.printDetails();

//В лог пишется размер пакета
//  sprintf(log, "size of packet   = %hu\n", sizeof(Packet));
//  writeLog(log);  

  _processPacket = processPacket;	//Присваивает указателю функцию processPacket(Packet)

}


void sendPacket(Packet packet)
{
	radio.stopListening();
	radio.openWritingPipe(base_pipe+packet.nodeId); //open pipe unique to the node
	radio.setAutoAck(0, false);
	radio.write( &packet, sizeof(Packet) );
//	printPacket("sendPacket", packet);

	radio.startListening();
}

void RF24Loop(void)
{

    if ( radio.available() )
    {
        Packet packet;
        radio.read( &packet, sizeof(Packet) );  

//		printPacket("Read from MiniRBoard: ", packet);

		_processPacket(packet);  //Выполняет функцию processPacket c аргументом packet
    }

}
