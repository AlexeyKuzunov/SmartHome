#include "RF24SN.h"

using namespace std;

void setup(void)
{
	freopen("/home/alex/My_RF24SN_Server/error.log", "w", stderr);
	freopen("/home/alex/My_RF24SN_Server/message.log", "w", stdout);
	mqttSetup();
	RF24Setup(&processPacket);
}

void processPacket(Packet packet)
{
	
	if (packet.packetType == PUBLISH_PACKET) processPublishPacket(packet);
	else if (packet.packetType == REQUEST_PACKET) processRequestPacket(packet);
	else
	{
//		writeLog("got strangely corrupted packet: ");
		printPacket("got strangely corrupted packet", packet);
	}	
	
}

void processPublishPacket(Packet packet)
{
//	printPacket("publish", packet);
	packet.packetType = PUBACK_PACKET;
	sendPacket(packet);
		
	mqttPublish(packet.nodeId, packet.sensorId, packet.value);
}

void processRequestPacket(Packet packet)
{
//	printPacket("уходит на Mosquitto", packet);

	packet.packetType = RESPONSE_PACKET;
	packet.value = mqttRequest(packet.nodeId, packet.sensorId);
	
//	printPacket("ответ Mosquitto", packet);

	sendPacket(packet);

}

int loop(void)
{
	setup();
	while(1) {
		RF24Loop();
		mqttLoop();
	}
	return 0;
}


int main(int argc, char** argv)
{
  pid_t parpid, sid;
	parpid = fork(); //создаем дочерний процесс
		if(parpid < 0) {
			exit(1);
		} else if(parpid != 0) {
      exit(0);
		}
	umask(0);//даем права на работу с фс
	sid = setsid();//генерируем уникальный индекс процесса
	if(sid < 0) {
  	exit(1);
  }
	if((chdir("/")) < 0) {//выходим в корень фс
  	exit(1);
  }
	
	close(STDIN_FILENO);//закрываем доступ к стандартным потокам ввода-вывода
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	return loop();
}
