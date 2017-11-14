#include "Algorithms.h"

int16_t pAtm = 0;                              // атмосферное давление
int16_t p4 = 0;                                // давление в контуре 4 

int16_t wheelsPressure[6] = { 0 };             // текущее давление в колесах
int16_t wheelsPressure2[6] = { 0 };            // предыдущее давление в колесах

int16_t requiredPressure;
uint8_t damagedWheels = 0;                     // Поврежденные колеса, их надо подкачать
uint8_t disabledWheels = 0;                    // Колеса, отключенные от управления

#define WHEELS_ALL 0xFF

											   ////////////////////  ПРИВАТНЫЕ ФУНКЦИИ ////////////////////////////////////////

											   /**
											   \brief Типо итератор.
											   Позволяет итерировать по битовой маске колес, только по "1".

											   \param i Предыдущее значение счетчика
											   \param mask Битовая маска
											   */
uint8_t GetNextWheel(uint8_t i, uint8_t mask)
{
	mask &= !damagedWheels & GetCircuitStatus();

	int size = WHEELS_COUNT;
	while (i < size && !(mask & (1 << i)))
		i++;

	return i;
}

/*
Реализует цикл с итератором.
Использование:
WHEELS_ITERATOR(i, my_mask)
...
*/
#define WHEELS_ITERATOR(I, MASK)for(uint8_t (I) = GetNextWheel(0, (MASK)); (I)<(WHEELS_COUNT); (I) = GetNextWheel((I)+1, (MASK)))




void A1_CheckValve(uint16_t valve, bool* kx_error, bool* k10_error);

// Измерение давления в 4м контуре
int16_t getCircuit4Pressure(void);

// Проверка, что давление нормальное
bool CheckCircuit4Pressure(void);

//измерение давление
//4 замера с промежутком 0.1 с, и затем усреднение
int16_t measurePressure(void);

//получение количества квантов подкачки
uint16_t calcUpQuants(uint16_t currentPressure, uint16_t requiredPressure);

// Получение количества квантов спуска
uint16_t calcDownQuants(uint16_t currentPressure, uint16_t requiredPressure);

// Держит клапана открытыми, постепенно закрывая, когда у колеса заканчивается
// время подкачки
void WaitForPressureDone(uint16_t* quantsNumber, uint16_t maxQuant, int quant);

void B1_ForTime(uint8_t wheels, int seconds);

////////////////////////////////////////////////////////////////////////////////

//Самодиагностика  системы при включении
AlgResult A0(void)
{
	AlgResult result;

	result = A1();
	if (result != ALG_OK)return result;

	result = A2();
	if (result != ALG_OK)return result;

	result = A3();
	if (result != ALG_OK)return result;

	result = A4();
	if (result != ALG_OK)return result;

	result = A5();
	if (result != ALG_OK)return result;

	A6(Wheels_All);

	return ALG_OK;
}

//Проверка работоспособности клапанов управления входным давлением К8 и К9
AlgResult A1(void)
{
	// Отказавшие клапана
	bool k8_error = false, k9_error = false, k10_error = false;
	bool critical_error = false;

	SetValves(Valves_None);

	// Проверка давления в контуре 4
	if (!CheckCircuit4Pressure())
		return ALG_CRITICAL_ERROR;


	/* Проверка клапана К8:
	После открытия давление должно увеличиваться,
	а после закрытия - вернуться к исходному*/
	A1_CheckValve(K8_Valve, &k8_error, &k10_error);

	if (!k10_error)
	{
		/* Проверка клапана К9.
		После открытия давление должно увеличиться,
		а потом вернуться */
		A1_CheckValve(K9_Valve, &k9_error, &k10_error);
	}

	// если отказали оба клапана К8 и К9 - критическая ошибка  
	critical_error = k10_error | (k8_error & k9_error);
	//CanSendK8K9K10Status(critical_error, k8_error, k9_error, k10_error);

	if (critical_error) return ALG_CRITICAL_ERROR;
	if (k8_error | k9_error) return ALG_ERROR;
	return ALG_OK;
}


// Проверяет либо К8, либо К9
void A1_CheckValve(uint16_t valve, bool* kx_error, bool* k10_error)
{
	int16_t p[2];

	p[0] = VDAT_0;                      // Амтосферное давление по паспорту
	SetValve(valve, VALVE_OPEN);
	uint8_t isIncreasing;

	vTaskDelay(1000);

	//Увеличилось ли давление?
	p[1] = GetAdcValue();
	isIncreasing = p[1] - p[0] > EPS_RISING;

	// если давление увеличивается
	if (isIncreasing)
	{
		SetValve(valve, VALVE_CLOSE);
		SetValve(K10_Valve, VALVE_OPEN);
		vTaskDelay(2000);

		if (abs(p[0] - GetAdcValue()) > EPS_EQUAL)
		{
			// если давление не вернулось к первоначальному состоянию - отказ клапана К10
			// критическая ошибка
			*k10_error = true;
		}

		SetValve(K10_Valve, VALVE_CLOSE);
	}
	else
	{
		// Отказ клапана К8
		*kx_error = true;
		SetValve(valve, VALVE_CLOSE);
	}
}



//Измерение атмосферного давления Р0 и самоадаптация к датчику давления
AlgResult A2(void)
{
	SetValves(Valves_None);
	SetValve(K10_Valve, VALVE_OPEN);

	vTaskDelay(3000);			//TODO: для отладки - 3000, реально - 30000

	int16_t pressure = measurePressure();
	bool isPressureOk = (pressure <= V1_1) && (pressure >= V0_4);	 // 0.4 - 1.1В

	//CanSendPressureSensorStatus(isPressureOk, pressure);

	if (isPressureOk)
		pAtm = pressure;

	SetValve(K10_Valve, VALVE_CLOSE);

	return isPressureOk ? ALG_OK : ALG_CRITICAL_ERROR;
}



//Измерение начального давления в питающей магистрали
AlgResult A3(void)
{
	bool isPressureOk;
	AlgResult result = ALG_OK;

	SetValves(Valves_None);
	SetValve(K8_Valve, VALVE_OPEN);
	SetValve(K9_Valve, VALVE_OPEN);

	int16_t p1 = 0, p2 = 0;
	TickType_t t0 = xTaskGetTickCount();

	// пока давление не поднимется до 6 кг/см2
	do
	{
		if (xTaskGetTickCount() - t0 >= 8000) //TODO: 5 * 60 * 1000
		{
			//Если за 5 минут не дошло, то некритическая ошибка
			//TODO: Не крит. ошибка. Отправить ошибку
			result = ALG_ERROR;
			break;
		}

		vTaskDelay(2500);
		p1 = GetAdcValue();
	} while (p1 < P_6KG_SM2);


	// пока давление не стабилизируется
	if (result == ALG_OK)
	{
		do
		{
			vTaskDelay(5000);
			p2 = GetAdcValue();
			isPressureOk = (p2 - p1 < DP_0_1KG_SM2);       // < 0.1 кг/см2  

			if (!isPressureOk)
			{
				p1 = p2;
			}

		} while (!isPressureOk);
	}

	// Возвращаем давление
	SetValve(K8_Valve, VALVE_CLOSE);
	SetValve(K9_Valve, VALVE_CLOSE);
	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	//CanSendInitialPressureStatus(p2);

	return result;
}



//Проверка герметичности БУД на избыточное давление
AlgResult A4(void)
{
	AlgResult result = ALG_OK;

	SetValves(Valves_None);

	SetValve(K8_Valve, VALVE_OPEN);
	SetValve(K9_Valve, VALVE_OPEN);
	vTaskDelay(500);
	SetValve(K8_Valve, VALVE_CLOSE);
	SetValve(K9_Valve, VALVE_CLOSE);
	vTaskDelay(2000);

	// замерить показания
	int16_t pressure1 = measurePressure();

	if (abs(pressure1 - p4) >= DP_0_2KG_SM2)      // < 0.2 кг/см2 
	{
		result = ALG_ERROR;
		// Не критическая ошибка. Но в чем ее суть?
		//TODO: Отправка статуса
	}


	vTaskDelay(4000); //TODO: 60 * 1000
	int16_t pressure2 = measurePressure();

	if (abs(pressure1 - pressure2) >= DP_0_1KG_SM2)
	{
		//Не герметичность БУД на избыточное давление
		result = ALG_CRITICAL_ERROR;
		//CanSendTigthnessPressureStatus(false);
	}
	else
		//CanSendTigthnessPressureStatus(true);


	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	return result;
}



//Проверка герметичности БУД на вакуум
AlgResult A5(void)
{
	AlgResult result = ALG_OK;

	SetValves(Valves_None);

	SetValve(K7_Valve, VALVE_OPEN);
	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(5000);
	SetValve(K10_Valve, VALVE_CLOSE);
	vTaskDelay(1000);
	SetValve(K7_Valve, VALVE_CLOSE);
	vTaskDelay(2000);

	// замерить показания
	int16_t pressure1 = measurePressure();

	if (pressure1 < pAtm)
	{
		vTaskDelay(6 * 1000);		//TODO: для отладки - 6000, реально - 60000
		int16_t pressure2 = measurePressure();

		if (pressure2 - pressure1 >= DP_0_05KG_SM2)
		{
			//Не герметичность буд на вакуум
			result = ALG_CRITICAL_ERROR;
			//CanSendTigthnessVakuumStatus(false);
		}
		else
		{
			//CanSendTigthnessVakuumStatus(true);
		}

		SetValve(K10_Valve, VALVE_OPEN);
		vTaskDelay(2000);
		SetValve(K10_Valve, VALVE_CLOSE);
	}
	else
	{
		result = ALG_CRITICAL_ERROR;
		//CanSendTigthnessVakuumStatus(false);
	}

	return result;
}



//Измерение текущего давления воздуха в шинах
void A6(uint8_t wheels)
{
	SetValves(Valves_None);

	for (int i = 0; i < 6; i++)
		WHEELS_ITERATOR(i, wheels)
	{
		// Pколi_2 = Pколi_тек
		wheelsPressure2[i] = wheelsPressure[i];

		vTaskDelay(500);

		//SetValves((1 << i) | K8_Valve | K9_Valve);
		SetValve(1 << i, VALVE_OPEN);
		SetValve(K8_Valve, VALVE_OPEN);
		SetValve(K9_Valve, VALVE_OPEN);

		vTaskDelay(900);

		SetValve(K8_Valve, VALVE_CLOSE);
		SetValve(K9_Valve, VALVE_CLOSE);

		vTaskDelay(2000);
		wheelsPressure[i] = measurePressure();
		SetValve(1 << i, VALVE_CLOSE);
	}

	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	//CanSendPressure(wheelsPressure);
}



//Работа системы в дежурном режиме
//Вызывается циклически из главной задачи
//values берутся из необходимого режима (???), пока подаются вручную
void B0(int16_t requiredWheelsPressure)
{
	// Счетчик подкачек подряд
	int pumpUpCnt = 0;

	//requiredWheelsPressure = reqPressure;

	A6(Wheels_All);

	// выполнение специальной процедуры "Определение повреждения колеса" Г0
	//G0();

	while (true)
	{
		uint8_t Kn = 0; //Маска, какие колеса подкачать
		uint8_t Ks = 0; //Маска, какие колеса спустить

						// Определение, какие колеса подкачать, а какие спустить
		WHEELS_ITERATOR(i, WHEELS_ALL)
		{
			if (requiredWheelsPressure - wheelsPressure[i] > DP_0_2KG_SM2)
				Kn |= 1 << i;
			else if (wheelsPressure[i] - requiredWheelsPressure > DP_0_2KG_SM2)
				Ks |= 1 << i;
		}

		if (Kn)
		{
			// Повышение
			//B1(Kn, requiredPressure);

			// Если повышение выполняется 3й раз подряд - задержка
			pumpUpCnt++;
			if (pumpUpCnt >= 3)
			{
				vTaskDelay(2000); //TODO: 2*60*1000
				return;
			}
		}
		else if (Ks)
		{
			//Снижение
			//B2(Ks, requiredPressure);
			pumpUpCnt = 0;
		}
		else
		{
			vTaskDelay(5000); //TODO: 5*60*1000
			return;
		}
	}
}



//Повышение давления воздуха в колесе (-ах)
void B1(uint8_t wheels, uint16_t requiredPressure)
{
	uint16_t quantsNumber[6] = { 0 };     // Время подкачки каждого колеса
	uint16_t maxQuant = 0;              // Максимальное время подкачки
	uint8_t currentUp = 0;

	// Определяем времена подкачек и формируем битовую маску, какие колеса подкачивать
	WHEELS_ITERATOR(i, wheels)
	{
		quantsNumber[i] = calcUpQuants(wheelsPressure[i], requiredPressure);
		if (quantsNumber[i] >= maxQuant)
			maxQuant = quantsNumber[i];

		if (quantsNumber[i])
			currentUp |= (1 << i);
	}

	// Открываем клапана
	SetValves(currentUp | K8_Valve | K9_Valve);

	// Подкачиваем
	WaitForPressureDone(quantsNumber, maxQuant, P_UP_QUANT);

	SetValves(Valves_None);

	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	if (A3() == ALG_OK)
		A6(Wheels_All);
}

// Подкачивает в течении заданного числа секунд
void B1_ForTime(uint8_t wheels, int seconds)
{
	// Открываем клапана
	WHEELS_ITERATOR(i, wheels)
		SetValve(ValveFromNumber(i), VALVE_OPEN);

	SetValve(K8_Valve, VALVE_OPEN);
	SetValve(K9_Valve, VALVE_OPEN);

	// Подкачиваем
	vTaskDelay(seconds * 1000);

	// Закрываем
	SetValve(K8_Valve, VALVE_CLOSE);
	SetValve(K9_Valve, VALVE_CLOSE);

	WHEELS_ITERATOR(i, wheels)
		SetValve(ValveFromNumber(i), VALVE_CLOSE);
}

//Снижение давления воздуха в колесе (-ах)
void B2(uint8_t wheels, uint16_t requiredPressure)
{
	uint16_t quantsNumber[6] = { 0 };     // Время спуска каждого колеса
	uint16_t maxQuant = 0;              // Максимальное время спуска
	uint8_t currentDown = 0;

	// Определяем времена спусков и формируем битовую маску, какие колеса спускаь
	WHEELS_ITERATOR(i, wheels)
	{
		quantsNumber[i] = calcDownQuants(wheelsPressure[i], requiredPressure);
		if (quantsNumber[i] >= maxQuant)
			maxQuant = quantsNumber[i];

		if (quantsNumber[i])
			currentDown |= (1 << i);
	}

	// Открываем клапана
	SetValves(currentDown | K7_Valve | K10_Valve);

	// Спускаем
	WaitForPressureDone(quantsNumber, maxQuant, P_DOWN_QUANT);

	SetValves(Valves_None);
	SetValves(K8_Valve | K9_Valve);
	vTaskDelay(700);
	SetValves(Valves_None);

	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);

	if (A3() == ALG_OK)
		A6(Wheels_All);
}


// Держит клапана открытыми, постепенно закрывая, когда у колеса заканчивается
// время подкачки
void WaitForPressureDone(uint16_t* quantsNumber, uint16_t maxQuant, int quant)
{
	for (int i = 0; i < maxQuant; i++)
	{
		vTaskDelay(quant);

		// Проверяем, какие колеса накачались и закрываем их
		for (int j = 0; j < 6; j++)
		{
			if (quantsNumber[j] != 0 && !(quantsNumber[j] > i + 1))
			{
				SetValve(1 << j, VALVE_CLOSE);
				quantsNumber[j] = 0;
			}
		}
	}
}


//Определение повреждения колеса
void G0(void)
{
	uint16_t wheelsPressure1[6] = { 0 };

	// Определяем клапаны, которые возможно спускают - между двумя последними 
	// измерениями их давление упало ниже порога
	uint8_t wheelsToCheck = 0;
	WHEELS_ITERATOR(i, WHEELS_ALL)
		wheelsToCheck |= (wheelsPressure2[i] - wheelsPressure[i] < DP_0_3KG_SM2) << i;   // < 0.3 кг/см2

	if (!wheelsToCheck)
		return;

	// Серия измерений
	A6(wheelsToCheck);

	WHEELS_ITERATOR(i, wheelsToCheck)
	{
		wheelsPressure1[i] = wheelsPressure[i];
	}

	vTaskDelay(10000);

	A6(wheelsToCheck);

	/* По результатм измерений:
	wheelsPressure  = Pti
	wheelsPressure2 = Pti-1
	wheelsPressure1 = Pti-2
	*/

	// Составление списка поврежденных колес
	damagedWheels = 0;
	WHEELS_ITERATOR(i, wheelsToCheck)
	{
		bool isDamaged = (wheelsPressure2[i] - wheelsPressure[i] > DP_0_1KG_SM2) || 		// > 0.1 кг/см2
			(wheelsPressure1[i] - wheelsPressure[i] > DP_0_15KG_SM2);       // > 0.15 кг/см2

		damagedWheels |= isDamaged << i;
	}

	if (damagedWheels)
	{
		//TODO: отправить статус о повреждении колесs
		D0(damagedWheels);
	}
}

void D0(uint8_t damagedWheels)
{

	uint16_t startPressures[6];
	WHEELS_ITERATOR(i, damagedWheels)
		startPressures[i] = wheelsPressure[i];

	B1_ForTime(damagedWheels, 2 * 60);
	A6(damagedWheels);
	B1_ForTime(damagedWheels, 2 * 60);
	A6(damagedWheels);

	do
	{

		// Определяем не накачивающиеся колесаы
		WHEELS_ITERATOR(i, damagedWheels)
		{
			if (startPressures[i] - wheelsPressure[i] > DP_0_5KG_SM2) // > 0.5 кг/см2
			{
				//Отключить колесо от управления
				disabledWheels |= (1 << i);

				// Исключить из списка поврежденных
				damagedWheels &= ~(1 << i);
			}
		}

		if (damagedWheels)
		{
			B1_ForTime(damagedWheels, 6 * 60);
			A6(damagedWheels);

			WHEELS_ITERATOR(i, damagedWheels)
			{
				if ((wheelsPressure[i] - requiredPressure >= DP_1KG_SM2)) // >= 1 кг/см2
				{
					// Исключить из списка поврежденных
					damagedWheels &= ~(1 << i);
				}
			}
		}

	} while (damagedWheels);

}

// Подкачка пробитых колес в течении заданного времени
void PumpUpDamagedWheels(uint8_t damagedWheels, TickType_t time)
{
	// Открываем клапана
	SetValves(damagedWheels | K8_Valve | K9_Valve);

	// Подкачиваем
	vTaskDelay(time);

	SetValves(Valves_None);

	SetValve(K10_Valve, VALVE_OPEN);
	vTaskDelay(2000);
	SetValve(K10_Valve, VALVE_CLOSE);
}

///////////////////// ПРИВАТНЫЕ ФУНКЦИИ ////////////////////////////////////////

//проверка давления в четвертом контуре
//ожидает, пока давление в 4 контуре не будет выше указанного порогового значения
int16_t getCircuit4Pressure(void)
{
	uint8_t buffer[COMMAND_LENGTH] = { 0 };
	int16_t circuit4Pressure;

	buffer[0] = CAN_CMD_Circuit_Request;
	CAN_Send_Command(buffer);
	CAN_GetContour4Response(&circuit4Pressure, portMAX_DELAY);

	p4 = circuit4Pressure;
	return circuit4Pressure;
}


// Проверка, что давление в контуре 4 нормальное
bool CheckCircuit4Pressure(void)
{
	//Проверка давления в контуре 4
	//Если давление в течении 5и минут не достигнет требуемого,
	//то критическая ошибка

	TickType_t t0 = xTaskGetTickCount();
	while (getCircuit4Pressure() <= NORMAL_PRESSURE)
	{
		// Прошло более 5и минут
		if (xTaskGetTickCount() - t0 > 5 * 60 * 1000)
		{
			return false;
		}

		vTaskDelay(1000);
	}

	return true;
}

//измерение давления
//4 замера с промежутком 0.1 с, и затем усреднение
int16_t measurePressure(void)
{
	uint16_t pressure = 0;
	for (int i = 0; i < 4; i++)
	{
		pressure += GetAdcValue();
		vTaskDelay(100);
	}
	pressure = pressure >> 2;
	return pressure;
}

//получение количества квантов подкачки
uint16_t calcUpQuants(uint16_t currentPressure, uint16_t requiredPressure)
{
	double numQuants = 1.0;
	double a = P_COMPRESSOR - requiredPressure;
	double b = P_COMPRESSOR - currentPressure;
	double c = log(a / b);
	double d = -52.0 * c;
	//numQuants = -206.0 * log((P_COMPRESSOR - requiredPressure) / (P_COMPRESSOR - wheelsPressure[wheel]));
	double e = (uint16_t)(floor(d) + 1);
	return e;
}

// Получение количества квантов спуска
uint16_t calcDownQuants(uint16_t currentPressure, uint16_t requiredPressure)
{
	return 0;
}
