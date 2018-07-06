#include "gpio.h"
#include "RF24.h"
#include "ASLibrary.h"
#include <iostream>
#include <cstdlib>

#define ce_gpio 5 //ce пин описан в script.fex как 5 пин gpio
#define SID 0 //идентификатор модуля

const uint64_t pipes[2] = {0xF0F0F0F0E1LL,0xF0F0F0F0E2LL};

RF24 radio(ce_gpio, "/dev/spidev0.0");


void init_nrf(void)
{
	// установка прерываний пока не используем
//	gpio_export(int_gpio_num);
//	gpio_set_edge(GPIO_STR, "rising", "1"); //отслеживаем переход из неактивного состояния  активное

	radio.begin(); //старт работы
	// enable dynamic payloads
//	radio.enableAckPayload(); //Разрешение отправки нетипового ответа передатчику
	radio.enableDynamicPayloads();
	radio.setAutoAck(RF24_AUTOACK); //Установка режима подтверждения приема
	radio.setRetries(5, 15); //Установка интервала и количества попыток "дозвона" до приемника
	radio.setDataRate(RF24_DATARATE); //Установка минимальной скорости
	radio.setPALevel(RF24_PA_LEVEL); //Установка максимальной мощности
	radio.setChannel(RF24_CHANNEL); //Установка канала вещания
	radio.setCRCLength(RF24_CRC_16);
	// Open pipes to other nodes for communication
	// Open pipe for reading
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]);
	
	//mkdir(DIR_PIPES, 0775);
	// Start listening
	//radio.startListening();
	radio.stopListening();
	// Dump the configuration of the rf unit for debugging
	radio.printDetails();
}

bool set_command(Message SetSensor) //отправляем команду на клиент
{
	radio.stopListening();
	return(radio.write(&SetSensor, sizeof(SetSensor)));
}

bool get_answer(SensorParam MySensor) //принимаем данные с клиента
{
	long started_time = __millis();
	bool timeout = false;
	bool ok = false;
	radio.startListening();
	
	while (!radio.available() && !timeout) 
		if (__millis() - started_time > 500) timeout = true;
	if (timeout) 
		printf("Failed, response timed out.\n\r");
	else
		{
		bool ok = radio.read(&MySensor, sizeof(MySensor));		
		}
	return ok;	
}

int main(int argc, char** argv)
{
	Message setCommand;

	SensorParam Sensor;

	setCommand.Id = SID; //SID модуля отправляющего команду
	setCommand.CommandTo = 1; //SID модуля которому отправляем команду
	setCommand.Command = 1; //Получаем значение
	setCommand.ParamID = 2; //получим значение температуры


	init_nrf();
	
	if (set_command(setCommand)) {
		printf("Packet sending ++++++\n\r");
		if (get_answer(Sensor))
		printf("%f\n", Sensor.Value);
		else printf("Error read\n\r");
	}
	else printf("Packet NOT sending -------------\n\r");
	return 0;
}
