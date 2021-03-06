﻿/*
 Name:		Nano.ino
 Created:	27.09.2017 19:49:46
 Author:	Алексей
*/

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include <timer-api.h>
#include <RF24_config.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "DHT.h"
#include "ASLibrary.h"


#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

// описание параметров модуля
#define SID 1                        // идентификатор модуля
#define NumSensors 2                   // количество сенсоров (и еще одно обязательное занчение - имя датчика)

SensorParam MySensors[NumSensors + 1] = {    // описание датчиков (и первичная инициализация)
	NumSensors,1,"Sensor DHT22",        // в поле "комментарий" указываем пояснительную информацию о датчике и количество сенсоров, 1 в статусе означает "отвечаем на запросы"
	0,0,"humidity",
	0,0,"temperature"	
};
Message Sensor;

// Определяем рабочие ножки;
RF24 radio(9, 10);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL };


void setup() {
	Serial.begin(9600);
	printf_begin();

	// частота=1Гц, период=1с
	timer_init_ISR_1Hz(TIMER_DEFAULT);

	NRF24_Init();
	dht.begin();
}

void loop() {
	bool done = false;
	getDHT();
	if (radio.available()) {
		//while (!done) {
		// Упираемся и
		while (!done) {
			done = radio.read(&Sensor, sizeof(Sensor));  // по адресу переменной in функция записывает принятые данные;
														 // если команда этому датчику - обрабатываем
			if (Sensor.CommandTo == SID) {
				// исполнить команду (от кого, команда, парметр, комментарий)
				doCommand(Sensor.Id, Sensor.Command, Sensor.ParamID, Sensor.ParamValue, Sensor.Status, Sensor.Comment);
			}
		}
	//	Serial.println(Sensor.Id);
	}
	
/*	
	// Wait a few seconds between measurements.
	delay(2000);

	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	float h = dht.readHumidity();
	// Read temperature as Celsius (the default)
	float t = dht.readTemperature();
	// Read temperature as Fahrenheit (isFahrenheit = true)
	float f = dht.readTemperature(true);

	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}

	// Compute heat index in Fahrenheit (the default)
	float hif = dht.computeHeatIndex(f, h);
	// Compute heat index in Celsius (isFahreheit = false)
	float hic = dht.computeHeatIndex(t, h, false);

	Serial.print("Humidity: ");
	Serial.print(h);
	Serial.print(" %\t");
	Serial.print("Temperature: ");
	Serial.print(t);
	Serial.print(" *C ");
	Serial.print(f);
	Serial.print(" *F\t");
	Serial.print("Heat index: ");
	Serial.print(hic);
	Serial.print(" *C ");
	Serial.print(hif);
	Serial.println(" *F");
*/
}

void NRF24_Init() {
	radio.begin();
	radio.setAutoAck(RF24_AUTOACK);
	// Разрешение отправки нетипового ответа передатчику;
	//radio.enableAckPayload();
	// enable dynamic payloads
	radio.enableDynamicPayloads();
	// optionally, increase the delay between retries & # of retries
	radio.setRetries(15, 15);
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
}

int serial_putc(char c, FILE *) {
	Serial.write(c);
	return c;
}

void printf_begin(void) {
	fdevopen(&serial_putc, 0);
}

void doCommand(unsigned char From, unsigned char Command, unsigned char ParamID, 
				float ParamValue, boolean Status, char* Comment) {
	switch (Command) {
	case 0:
		// ничего не делаем 
		break;
	case 1:
		// читаем и отправляем назад
		sendSlaveMessage(From, ParamID);
		break;
	case 2:
		// устанавливаем
		setValue(From, ParamID, ParamValue, Comment);
		break;
	default:
		break;
	}
	// ответим "командующему"
	//sendSlaveMessage(From, ParamID);

	return;
}

void sendSlaveMessage(unsigned char From, unsigned char ParamID) {
	switch (ParamID)
	{
		radio.stopListening();
	case 1:
		//Давление
		radio.write(&MySensors[1], sizeof(MySensors[1]));
		break;
	case 2:
		//Температура
		radio.write(&MySensors[2], sizeof(MySensors[2]));
		break;
	default:
		break;
	}
	//Отправляем значение параметра запросившему
	radio.startListening();
}

void setValue(unsigned char From, unsigned char ParamID, float ParamValue, char* Comment) {

}

void getDHT(void) {
	Serial.println("goodbye from timer");
	MySensors[1].Status = 0;
	MySensors[1].Value = dht.readHumidity();
	if (isnan(MySensors[1].Value)) {
		MySensors[1].Status = 1;
		Serial.println("Failed to read from DHT sensor!");
	}

	//MySensors[1].Comment = "C":
	MySensors[2].Status = 0;
	MySensors[2].Value = dht.readTemperature();
	if (isnan(MySensors[2].Value)) {
		MySensors[2].Status = 1;
		Serial.println("Failed to read from DHT sensor!");
	}
}

void timer_handle_interrupts(int timer) {
	getDHT();
}