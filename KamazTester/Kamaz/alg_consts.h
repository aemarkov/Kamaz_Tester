#ifndef __ALGCONSTS_H__
#define __ALGCONSTS_H__

#include <stdint.h>
//#include <FreeRTOS.h>
#include <API\api.h>

/* Формулы для перевода давления
	мин: - 1атм	 0.6в
	макс:  9атм  4.6в
	АЦП [0; 5в] -> [0; 4096]
	
	

	если в Вольтах         N = V / 5.1 * 4096
	если в атм (кг/см2)    N = (p + 1) * 321 + 482
	если разница давлений  N = (dp / 1) * 321
*/

//#define TickType_t uint32_t

#define V_ADC(v)((v) / 5.1 * 4096)
#define PRESS_ADC(atm)(((atm) + 1) * 321 + 482)
#define DIFF_ADC(atm)((atm)*321)

extern const int16_t EPS_EQUAL;
extern const int16_t EPS_RISING;
extern const int16_t NORMAL_PRESSURE;
extern const int16_t V1_1;
extern const int16_t V0_4;
extern const int16_t P_6KG_SM2;
extern const int16_t DP_0_1KG_SM2;
extern const int16_t DP_0_2KG_SM2;
extern const int16_t DP_0_05KG_SM2;
extern const TickType_t T_PRES_UP_MIN;
extern const TickType_t T_PRES_UP_MAX;
extern const TickType_t T_PRES_DOWN_MIN;
extern const TickType_t T_PRES_DOWN_MAX;
extern const int16_t DP_0_3KG_SM2;
extern const int16_t DP_0_15KG_SM2;
extern const int16_t DP_0_5KG_SM2;
extern const int16_t DP_1KG_SM2;

extern const TickType_t P_UP_QUANT;
extern const TickType_t P_DOWN_QUANT;
extern const int16_t P_COMPRESSOR;
extern const TickType_t T_PAUSE;

extern const int16_t VDAT_0;

#endif
