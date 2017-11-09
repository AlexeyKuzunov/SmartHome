#ifndef __ASLibrary__
#define __ASLibrary__

#define COMMENTLEN		16		//Длина комментария в сообщении
#define RF24_CHANNEL		76		// канал, на котором работает трансивер NRF24L01 RF channel for the sensor net, 0-127
#define RF24_DATARATE		RF24_250KBPS	//RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
#define RF24_PA_LEVEL		RF24_PA_MAX	//Sensor PA Level == RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm
#define RF24_PA_LEVEL_GW	RF24_PA_LOW	//Gateway PA Level, defaults to Sensor net PA Level.  Tune here if using an amplified nRF2401+ in your gateway.
#define RF24_PAYLOAD 		5		//Максимальный размер буфера в байтах для трансивера NRF24L01
						//должен буть не меньше максимальной длины передаваемых данных
						//и не больше 32 (стркутуры _AS_COMMAND и _AS_ANSWER)
#define RF24_AUTOACK		0		//Аппаратное подтверждение приема





//Запрос от сервера
typedef struct _MESSAGE {
	unsigned char	Id;			//идентификатор клиента. Номер ячейки с адресом канала
	unsigned char	CommandTo;		// команда модулю номер ...
	unsigned char	Command;		//Команда
						// 0 - нет команды или ответ
						// 1 - получить значение
						// 2 - установить значение
	unsigned char	ParamID;		// идентификатор параметра
	float		ParamValue;		// значение параметра
	bool		Status;			// статус 0 - ошибка, 1 - ок
	char		Comment[COMMENTLEN];	// комментарий
} Message;

// Состояние датчиков
typedef struct _SENSORPARAM
{
	unsigned char	Status;			//Статус
	float		Value;			//Значение
	char		Comment[COMMENTLEN];	//Описание
} SensorParam;

#endif // __ASLibrary__
