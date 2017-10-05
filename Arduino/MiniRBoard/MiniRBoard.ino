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
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL };
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
	radio.openWritingPipe(pipes[1]);
	radio.openReadingPipe(1, pipes[0]);
	// Start listening
	radio.startListening();
	// Dump the configuration of the rf unit for debugging
	radio.printDetails();
	//attachInterrupt(0, check_radio, FALLING); //прерывание
}


// the loop function runs over and over again until power down or reset
void loop() {
	char in;
	//bool done = false;                            //Вспомогательная переменная;
	if (radio.available()) {
		//while (!done) {                            // Упираемся и
			radio.read(&in, sizeof(in));  // по адресу переменной in функция записывает принятые данные;
			Serial.println(in);
	}
		
	
}

int serial_putc(char c, FILE *) {
	Serial.write(c);
	return c;
}

void printf_begin(void) {
	fdevopen(&serial_putc, 0);
}
