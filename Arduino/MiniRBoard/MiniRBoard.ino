/* 
Name:		MiniRBoard.ino
Created:	27.05.2017 22:08:56
 Author:	Алексей
*/

// the setup function runs once when you press reset or power the board
#include <RF24_config.h>
#include <OneWire.h>
#include <RF24_config.h>
#include <RF24.h>
//#include <printf.h>
#include <nRF24L01.h>
#include "ASLibrary.h"

#define TIMEOUT 400 // таймаут запросов от сервера.
#define NUMSENSORS 2	//
#define IDCLIENT 1	// номер клиента 1...

OneWire ds(3);

// Определяем рабочие ножки;
RF24 radio(9, 10);
//const uint64_t pipe = 0xE8E8F0F0E1LL; // Определяем адрес рабочей трубы;
const uint64_t pipes[2] = { 0xF0F0F0F0E2LL, 0xF0F0F0F0E1LL };
const unsigned char id = 1; //идентификатор клиента. Номер ячейки с адресом канала
volatile boolean flag_tx, flag_fail, flag_rx;
unsigned long errorstate = 0;
int Relay = 4;
boolean stateRelay = LOW;

AS_SensorStatus MySensors[NUMSENSORS] = {
	0,0,"18B20_temp(C)",
	0,0,"Swith" };

float Get_18B20_Data() {
	byte DSdata[2];
	ds.reset();
	ds.write(0xCC);
	ds.write(0x44);
	delay(1000);
	ds.reset();
	ds.write(0xCC);
	ds.write(0xBE);
	DSdata[0] = ds.read();
	DSdata[1] = ds.read();
	int Temp = (DSdata[1] << 8) + DSdata[0];
	return (float)Temp / 16;
}

// Переключает реле
void Switch() {
	stateRelay = !stateRelay;
	digitalWrite(Relay, stateRelay);
}

//Вычисляет все данные и заполняет массив значений датчиков
byte CalculateAllData() {
	MySensors[0].Status = 1;
	MySensors[0].Value = Get_18B20_Data();
	MySensors[1].Status = 1;
	MySensors[1].Value = stateRelay;
	//Serial.print(MySensors[2].Value);
	return 1;
}

void check_radio() { // обработчик прерывания
	bool tx, fail, rx;
	radio.whatHappened(tx, fail, rx); // читаем регистр
	flag_tx = tx; // 1 если успешно отправили данные
	flag_fail = fail; // 1 если данные не отправленны   
	flag_rx = rx;  // 1 если пришли данные для чтения 
	//Serial.println(rx);
}

AS_Answer ExecuteCommand(AS_Command MyCommand) {
	AS_Answer  MyAnswer;  // Ответ
	MyAnswer.Status = 0;
	MyAnswer.Value = 0;
	if (id == MyCommand.id) {
		MyAnswer.Id = IDCLIENT;
		switch (MyCommand.Command) {
		case 1: //Получить количество датчиков
			MyAnswer.Status = 1;
			MyAnswer.Value = NUMSENSORS;
			MyAnswer.Count = errorstate;
			memcpy(&MyAnswer.Comment, &"Sun sensors.", COMMENTLEN);
			//Serial.println("1.");
			break;
		case 2: //Рассчитать все значения датчиков
			MyAnswer.Status = CalculateAllData();
			MyAnswer.Count = errorstate;
			///Serial.println("2.");
			break;
		case 3:
			Switch();
			MyAnswer.Status = 1;
			MyAnswer.Value = stateRelay;
			MyAnswer.Count = errorstate;
			break;
		case 4: //Получиь значение датчика по номеру
				//Serial.println("4.");
			if ((MyCommand.Parametr + 1) > NUMSENSORS) {
				MyAnswer.Status = 0;
				MyAnswer.Value = 0;
				MyAnswer.Count = errorstate;
				memcpy(&MyAnswer.Comment, &"Too big num par.", COMMENTLEN);
			}
			else {
				MyAnswer.Status = MySensors[MyCommand.Parametr].Status;
				MyAnswer.Value = MySensors[MyCommand.Parametr].Value;
				MyAnswer.Count = errorstate;
				memcpy(&MyAnswer.Comment, &MySensors[MyCommand.Parametr].Comment, COMMENTLEN);
				//MyAnswer.Comment[0] = P[0];
			}
			break;
		}
	}
	return MyAnswer;
}

void setup() {
	Serial.begin(57600);
	printf_begin();
	radio.begin();
	radio.setAutoAck(false);
	// Разрешение отправки нетипового ответа передатчику;
	//radio.enableAckPayload();
	// enable dynamic payloads
	//radio.enableDynamicPayloads();
	// optionally, increase the delay between retries & # of retries
	//radio.setRetries(15, 15);
	radio.setDataRate(RF24_DATARATE);
	radio.setChannel(RF24_CHANNEL);
	radio.setPALevel(RF24_PA_LEVEL);
	// Open pipes to other nodes for communication
	// Open 'our' pipe for writing
	// Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]);
	// Start listening
	radio.startListening();
	// Dump the configuration of the rf unit for debugging
	//radio.printDetails();
	attachInterrupt(0, check_radio, FALLING);
}

// the loop function runs over and over again until power down or reset
void loop() {
	AS_Answer MyAnswer;
	AS_Command MyCommand;
//	CalculateAllData();
	if (flag_rx) { // если пришли данные для чтения
		radio.read(&MyCommand, sizeof(MyCommand));
		
		Serial.print("id: ");
		Serial.println(MyCommand.Id);
		Serial.print("Command: ");
		Serial.println(MyCommand.Command);
		Serial.print("Parametr: ");
		Serial.println(MyCommand.Parametr);

/*		MyAnswer = ExecuteCommand(command);
		
		Serial.print("id: ");
		Serial.println(MyAnswer.Id);
		Serial.print("Status: ");
		Serial.println(MyAnswer.Status);
		Serial.print("Count: ");
		Serial.println(MyAnswer.Count);
		Serial.print("Value: ");
		Serial.println(MyAnswer.Value);
		Serial.print("Comment: ");
		Serial.println(MyAnswer.Comment);
		Serial.print("Sizeof_Answer: ");
		Serial.println(sizeof(MyAnswer));
		
		radio.stopListening();		
		radio.write(&MyAnswer, sizeof(MyAnswer));
		radio.startListening();
*/

		flag_rx = 0;
		//Serial.println(command.id);
	}
	if (flag_fail) { // если данные не отправленны
		flag_fail = 0;
		errorstate++;
	}

	if (flag_tx) { // если данные отправленны 
		flag_tx = 0;

	}
}

int serial_putc(char c, FILE *) {
	Serial.write(c);
	return c;
}

void printf_begin(void) {
	fdevopen(&serial_putc, 0);
}
