#include "api.h"

uint16_t valvesState;			// Маска клапанов
uint16_t circuitState;			// Маска обрывов

float wheelsPressures[6];		// Давления в колесах
float budPressure;				// Давление в полости (== давление датчика)
float compressorPressure;		// Давление в контуре
float atmPressure;				// Давление в атмосфере

int currentTime;				// Текущее время в тиках

vector<string> actions;

vector<string> & getActions()
{
	return actions;
}

// Преобразует давление в атмосферах в значение АЦП
int16_t atmToAdc(float atm)
{
	return (atm + 1) * 321 + 482;
}

// Преобразует значение АЦП в давление в атмосферах
float adcToAtm(int16_t adc)
{
	return (adc - 482) / 321 - 1;
}

// Возвращает маску от списка открытости клапанов
uint16_t getValvesMask(vector<bool> valves)
{
	uint16_t mask = 0;
	for (int i = 0; i < 10; i++)
		mask |= (1 << i);

	return mask;
}

//Устанавливает открытость клапанов 
void setValves(vector<bool> valves)
{
	valvesState = getValvesMask(valves);
}

// выводит состояние клапанов
string valvesToString(uint16_t valves)
{
	stringstream s;
	bool isOpen;

	for (int i = 0; i < 9; i++)
	{
		isOpen = (valvesState & (1 << i)) != 0;
		s << "K" << i + 1 << ": " << isOpen << " ";
	}

	isOpen = (valvesState & (1 << 9)) != 0;
	s << "K" << 10 << ": " << isOpen << " ";

	return s.str();
}

// выводит состояние клапанов
string valvesToString()
{
	return valvesToString(valvesState);
}


/////////////////////////// RTOS /////////////////////////////////////////
void vTaskDelay(int delay)
{	
	currentTime += delay;
	actions.push_back("delay " + std::to_string(delay));
}

TickType_t xTaskGetTickCount(void)
{
	return currentTime;
}


/////////////////////////// VALVES ///////////////////////////////////////
//Возвращает клапан по номеру
uint16_t ValveFromNumber(uint8_t valveNumber)
{
	//У каждого клапана значение - битовая маска, где 1 на номере клапана
	//Если изменится в конфиге, то все сломается
	return 1 << (valveNumber - 1);
}

//Возвращает номер по клапану
uint8_t NumberFromValve(uint16_t valve)
{
	// Номера от 0 до 9
	uint8_t number;
	switch (valve)
	{
	case K1_Valve:
		number = 0;
		break;
	case K2_Valve:
		number = 1;
		break;
	case K3_Valve:
		number = 2;
		break;
	case K4_Valve:
		number = 3;
		break;
	case K5_Valve:
		number = 4;
		break;
	case K6_Valve:
		number = 5;
		break;
	case K7_Valve:
		number = 6;
		break;
	case K8_Valve:
		number = 7;
		break;
	case K9_Valve:
		number = 8;
		break;
	case K10_Valve:
		number = 9;
		break;
	}
	return number;
}

//Открывает и закрывает клапана на основе битовой маски
void SetValves(uint16_t valves)
{
	valvesState = valves;
	actions.push_back(valvesToString());
}

//Открывает/закрывает один клапан
void SetValve(uint16_t valve, uint8_t state)
{
	if (state == VALVE_OPEN)
		valvesState |= valve;
	else if(state == VALVE_CLOSE)
		valvesState &= ~valve;

	actions.push_back(valvesToString());
}


/////////////////////////// VALVES POWER /////////////////////////////////
uint16_t GetCircuitStatus(void)
{
	actions.push_back("get circuit status");
	return circuitState;
}


/////////////////////////// ADC //////////////////////////////////////////
int16_t GetAdcValue(void)
{
	actions.push_back("adc");
	return atmToAdc(budPressure);
}


/////////////////////////// CAN //////////////////////////////////////////
long CAN_GetContour4Response(void* pvBuffer, TickType_t xTicksToWait)
{
	int16_t pressure = atmToAdc(compressorPressure);
	memcpy(pvBuffer, &pressure, sizeof(pressure));
	actions.push_back("get contour 4 response");

	return 0;
}

void CAN_Send_Command(uint8_t* buffer)
{
	actions.push_back("send contour 4 request");
}


void CanJ1939_SendCircuit4Error()
{
	actions.push_back("j1939 circuit 4 error");
}

void CanJ1939_SendK10Error()
{
	actions.push_back("j1939 k10 error");
}

void CanJ1939_SendK8AndK9Error()
{
	actions.push_back("j1939 k8 k9 error");
}

void CanJ1939_SendK8Error()
{
	actions.push_back("j1939 k8 error");
}

void CanJ1939_SendK9Error()
{
	actions.push_back("j1939 k9 error");
}

void CanJ1939_SendSensorNotCalibratedError()
{
	//TRIGGERET
	actions.push_back("j1939 sensor not callibrated error");
}

void CanJ1939_SendPressureNotEnoughFor5MinutesError()
{
	actions.push_back("j1939 pressure not enough error");
}

void CanJ1939_SendTightnessPressureError()
{
	actions.push_back("j1939 tightness pressure error");
}

void CanJ1939_SendTightnessVakuumError()
{
	actions.push_back("j1939 tightness vakuum error");
}