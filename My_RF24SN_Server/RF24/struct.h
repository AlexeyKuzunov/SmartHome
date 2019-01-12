// Пакет передаваемый, используется также для хранения результатов.
	struct type_packet_0x20_NRF24   // Версия 2.4!! адаптация для stm32 Структура передаваемого пакета 32 байта - 32 максимум
	    {
	        byte id=0x00;                           // Идентификатор типа устройства - старшие 4 бита, вторые (младшие) 4 бита серийный номер устройства
	        byte DHT_error;                         // Ошибка разряды: 0-1 первый датчик (00-ок) 2-3 второй датчик (00-ок) 4 - радиоканал    
	        int16_t   tOut=-500,tIn=500;            // Текущие температуры в сотых градуса !!! место экономим
	        uint16_t  absHOut=123,absHIn=123;       // Абсолютные влажности в сотых грамма на м*3 !!! место экономим
	        uint16_t  relHOut=123,relHIn=123;       // Относительные влажности сотых процента !!! место экономим
	        uint8_t   flags=0x00;                   // байт флагов 
	                                                // 0 бит - мотор включен/выключен
	                                                // 1 бит - нагреватель включен/выключен
	                                                // 2 бит -[1 - dH_min задается в сотых грамма на м*3] [0 - dH_min заадется в ДЕСЯТЫХ процента от absHIn]
	                                                // 3 4 - пока пусто
	                                                // 5-7 - номер настройки = settingRAM.mode до 8 настроек, надо передавать, что бы на приемнике восстановить
	        uint8_t   dH_min;                       // Порог включения вентилятора по РАЗНИЦЕ абсолютной влажности в сотых грамма на м*3 или в ДЕСЯТЫХ % см flags:2
	        uint8_t   T_min;                        // Порог выключения вентилятора по температуре в ДЕСЯТЫХ долях градуса, только положительные значения
	        uint8_t   count=0;                      // циклический счетчик отправленных пакетов нужен что бы на приемнике проверять качество связи
	        char note[14] = "NONE";                 // Примечание не более 13 байт + 0 байт Русские буквы в два раза меньше т.к. UTF-8
	    } packet_0x20[NUM_DRY]; 