//#include <Arduino.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "RF24SN.h"
#include <OneWire.h>
#include "Relay.h"
#include "Thread.h"


#define RF24SN_NODE_ID 2
#define RF24SN_SERVER_ADDR 0xF0F0F0F000LL
#define TEMP_SENSOR 2
#define RELAY_SENSOR 1
#define BTN_PIN A0							//пин с кнопкой

RF24 radio(9,10); // Для Уно
RF24SN rf24sn( &radio, RF24SN_SERVER_ADDR, RF24SN_NODE_ID);

Relay light(4, false); // constructor receives (pin, isNormallyOpen) true = Normally Open, false = Normally Closed
OneWire  ds(3);  // on pin 10 (a 4.7K resistor is necessary)

bool sw = 1;				//хранит значение реле

Thread TempPubThread = Thread();
Thread SwitchThread = Thread();


bool GetKeyValue();
float convert(byte data[12], byte type_s);
float ReadTemp();
void TempPublish(void);
void Switch(void);
bool Request(void);
void BtnClick(void);
//void check_radio(void);
long publishTimer, requestTimer;
//uint16_t publishTimer, requestTimer;
//float add;
//bool RelayStatus;

void setup(void){
	Serial.begin(57600);
	Serial.println();

	pinMode(BTN_PIN, INPUT);

	//Настройка радио
	printf_begin(); //необходима для отображения информации через PrintDetails 
	//	radio.setAutoAck(false);
	rf24sn.begin();
	//	rf24sn.printDetails();
	//	radio.stopListening();

	light.begin(); //Настройка реле

	TempPubThread.onRun(TempPublish);
	TempPubThread.setInterval(5000);

	SwitchThread.onRun(Switch);
	SwitchThread.setInterval(1000);
}

void loop(void) {
	if (TempPubThread.shouldRun())
		TempPubThread.run();
	if (SwitchThread.shouldRun())
		SwitchThread.run();
}

void Switch(void) {

if (GetKeyValue() == 0) {
	sw = !sw;
	Serial.println(sw);
}

}

bool GetKeyValue() { // Функция устраняющая дребезг 
	static bool oldKeyValue; // Переменная для хранения предыдущего значения состояния кнопок
	static long lastChange; // Переменная для хранения времени последнего изменения состояния 
	bool actualKeyValue = digitalRead(BTN_PIN); // Получаем актуальное состояние 
	if ((actualKeyValue != oldKeyValue) && (millis() - lastChange > 200)) { // Пришло новое значение, и с последнего изменения прошло достаточно времени 
		oldKeyValue = actualKeyValue; // Запоминаем новое значение 
		lastChange = millis(); // Обнуляем таймер 
	} 
	return oldKeyValue; // Отправляем старое, либо уже модифицированное новое значение
}

//запрос последнего значения темы RF24SN
bool Request(void) {
	
	float relay;
	
	bool requestSuccess = rf24sn.request(RELAY_SENSOR, &relay);
	if (requestSuccess) {
		//Serial.print("Got response. From now on, I'll be adding ");
		//Serial.println(relay);
		return relay;
	} else {
		Serial.println("Request failed");
	}
}

void BtnClick(void){
	//bool a = analogRead(BTN_PIN);
	bool a = digitalRead(BTN_PIN);
	Serial.println(a);


	if(a == 0){
		//delay (50);
		sw =  0;
		//light.turnOn(); //Включаеим реле
	} else {
			sw = 1;
		//light.turnOff(); //Включаеим реле
	}

}

void TempPublish(void) {

rf24sn.publish(TEMP_SENSOR, ReadTemp());

/*
	bool TempPublishSuccess = rf24sn.publish(TEMP_SENSOR, ReadTemp());
	if (TempPublishSuccess) {
		Serial.println("Publish Temperature OK");
	} else {
		Serial.println("Publish Temperature failed");
	}
*/
}

void RelayPublish(void) {
	
	bool RelayPublishSuccess = rf24sn.publish(RELAY_SENSOR, (int)light.getState());
	if (RelayPublishSuccess) {
//		Serial.println("Publish Relay OK");
	} else {
//		Serial.println("Publish Relay failed");
	}
}

//данные о температуре из DS18x20 преобразуем в темп. по цельсию
float convert(byte data[12], byte type_s){ 
	int16_t raw = (data[1] << 8) | data[0];
	if (type_s) {
		raw = raw << 3; // 9 bit resolution default
		if (data[7] == 0x10) {
			// "count remain" gives full 12 bit resolution
			raw = (raw & 0xFFF0) + 12 - data[6];
		}
	} else {
		byte cfg = (data[4] & 0x60);
		// at lower res, the low bits are undefined, so let's zero them
                if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
		else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
	                else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
			// default is 12 bit resolution, 750 ms conversion time
	}
	return (float)raw / 16.0;
}


float ReadTemp(){
	byte type_s;
	byte data[12];
	byte addr[8];
	byte i;
	  
	while ( !ds.search(addr)) {
		//Serial.println("No more addresses.");
		//Serial.println();
		ds.reset_search();
		delay(250);
		//return 999;
	}
	if (OneWire::crc8(addr, 7) != addr[7]) {
		Serial.println("CRC is not valid!");
		return 9999;
	}
	switch (addr[0]) {
		case 0x10:
			//Serial.println("  Chip = DS18S20");  // or old DS1820
			type_s = 1;
			break;
		case 0x28:
			//Serial.println("  Chip = DS18B20");
			type_s = 0;
			break;
		case 0x22:
		//	Serial.println("  Chip = DS1822");
			type_s = 0;
			break;
		default:
			Serial.println("Device is not a DS18x20 family device.");
			return 99999;
	}
	ds.reset();
	ds.select(addr);
	ds.write(0x44, 1); // start conversion, with parasite power on at the end
	delay(1000);
	ds.reset();
	ds.select(addr);    
	ds.write(0xBE);         // Read Scratchpad
	for ( i = 0; i < 9; i++) {           // we need 9 bytes
		data[i] = ds.read();
		//Serial.print(data[i], HEX);
		//Serial.print(" ");
	}
	return convert(data, type_s);  
}
