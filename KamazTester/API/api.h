#ifndef __API_H__
#define __API_H__


#include <stdint.h>
#include <vector>
#include <string>
#include <sstream>
#include "InputStore.h"

using namespace std;


#define NUM_WHEELS 6

#define K1_Valve  1
#define K2_Valve  2
#define K3_Valve  4
#define K4_Valve  8
#define K5_Valve  16
#define K6_Valve  32
#define K7_Valve  64
#define K8_Valve  128
#define K9_Valve  256
#define K10_Valve 512

#define Valves_All  0x03FF
#define Valves_None 0

#define Wheels_All 0x3F

#define VALVE_OPEN   1
#define VALVE_CLOSE  0


#define TickType_t int


#define COMMAND_LENGTH                      8       //Число байт в команде
#define RESULT_LENGTH                       8       //Число байт в ответе
#define CAN_CMD_Circuit_Request             1
#define portMAX_DELAY						0xffffffffUL



#define DELAY "delay "
#define GET_CIRCUIT_STATUS "get circuit status"
#define ADC "adc"
#define GET_CONTOUR_4_RESPONSE "get contour 4 response"
#define SEND_CONTOUR_4_REQUEST "send contour 4 request"
#define J1939_CIRCUIT_4_ERROR "j1939 circuit 4 error"
#define J1939_K10_ERROR "j1939 k10 error"
#define J1939_K8_K9_ERROR "j1939 k8 k9 error"
#define J1939_K8_ERROR "j1939 k8 error"
#define J1939_K9_ERROR "j1939 k9 error"
#define J1939_SENSOR_NOT_CALIBRATED_ERROR "j1939 sensor not calibrated error"
#define J1939_PRESSURE_NOT_ENOUGH_ERROR "j1939 pressure not enough error"
#define J1939_TIGHTNESS_PRESSURE_ERROR "j1939 tightness pressure error"
#define J1939_TIGHTNESS_VAKUUM_ERROR "j1939 tightness vakuum error"


/////////////////////////// RTOS /////////////////////////////////////////
void vTaskDelay(int delay);
TickType_t xTaskGetTickCount(void);


/////////////////////////// VALVES ///////////////////////////////////////
//Возвращает клапан по номеру
uint16_t ValveFromNumber(uint8_t valveNumber);

//Возвращает номер по клапану
uint8_t NumberFromValve(uint16_t valve);

//Открывает и закрывает клапана на основе битовой маски
void SetValves(uint16_t valves);

//Открывает/закрывает один клапан
void SetValve(uint16_t valve, uint8_t state);


/////////////////////////// VALVES POWER /////////////////////////////////
uint16_t GetCircuitStatus(void);


/////////////////////////// ADC //////////////////////////////////////////
int16_t GetAdcValue(void);


/////////////////////////// CAN //////////////////////////////////////////
long CAN_GetContour4Response(void* pvBuffer, TickType_t xTicksToWait);
void CAN_Send_Command(uint8_t* buffer);

void CanJ1939_SendCircuit4Error();

void CanJ1939_SendK10Error();
void CanJ1939_SendK8AndK9Error();
void CanJ1939_SendK8Error();
void CanJ1939_SendK9Error();

void CanJ1939_SendSensorNotCalibratedError();
void CanJ1939_SendPressureNotEnoughFor5MinutesError();
void CanJ1939_SendTightnessPressureError();
void CanJ1939_SendTightnessVakuumError();

////////////////////////// Системные ////////////////////////////////////

void ResetEnv();
vector<string> & getActions();

// Возвращает маску от списка открытости клапанов
uint16_t getBoolMask(vector<bool> valves);
string valvesToString(uint16_t valves);

// Входные параметры
extern InputStore<uint16_t> circuitState;			// Маска обрывов
extern InputStore<float> budPressure;				// Давление в полости (== давление датчика)
extern InputStore<float> contour4Pressure;			// Давление в контуре

#endif