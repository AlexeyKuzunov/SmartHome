﻿/* 
Name:		MiniRBoard.ino
Created:	27.05.2017 22:08:56
 Author:	Алексей
*/

// the setup function runs once when you press reset or power the board
#include <RF24_config.h>
#include <RF24.h>
//#include <printf.h>
#include <nRF24L01.h>
#include "ASLibrary.h"

#define timeoutper 400 // таймаут запросов от сервера.

AS_Command Command;
// Определяем рабочие ножки;
RF24 radio(9, 10);
//const uint64_t pipe = 0xE8E8F0F0E1LL; // Определяем адрес рабочей трубы;
const uint64_t pipes[2] = { 0xF0F0F0F0E2LL, 0xF0F0F0F0E1LL };

void setup() {
	Serial.begin(57600);
	printf_begin();
	radio.begin();
	radio.setAutoAck(false);
	// Разрешение отправки нетипового ответа передатчику;
	//radio.enableAckPayload();
	// enable dynamic payloads
	radio.enableDynamicPayloads();
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
	radio.printDetails();
}

// the loop function runs over and over again until power down or reset
void loop() {
	char receive_payload[] = "";
	unsigned long started_waiting_at = millis();
	bool timeout = false;
	while (!radio.available() && !timeout)
		if (millis() - started_waiting_at > timeoutper) timeout = true;
	if (timeout) {
		//  счетчик ошибок
		//clientnf.Error_Message++;
		//errorstate++; // счетчик ошибок для повтора
		
	}
	else
	{
		int len = radio.getDynamicPayloadSize();
		radio.read(receive_payload, len);
		Serial.print(receive_payload);
	}
}

int serial_putc(char c, FILE *) {
	Serial.write(c);
	return c;
}

void printf_begin(void) {
	fdevopen(&serial_putc, 0);
}
