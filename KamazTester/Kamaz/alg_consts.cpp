#include "alg_consts.h"


const int16_t EPS_EQUAL             = 200;                     // погрешность возвращения давления к норме (А1)
const int16_t EPS_RISING            = 30;                      // погрешность увеличение давления (А1)
const int16_t NORMAL_PRESSURE       = PRESS_ADC(4);            // нормальное давление, 4 кг/см2 (A1)
const int16_t V1_1                  = V_ADC(1.1);              // верхняя и нижняя границы атмосферного давления (А2)
const int16_t V0_4                  = V_ADC(0.4);              // 0.4 - 1.1В        	   
const int16_t P_6KG_SM2             = PRESS_ADC(6);            // 6 кг/см2  (А3)
const int16_t DP_0_1KG_SM2          = DIFF_ADC(0.1);           // разница в 0.1 кг/см2(A3, A4, Г0)
const int16_t DP_0_2KG_SM2          = DIFF_ADC(0.2);           // разница в 0.2 кг/см2(A4, В0)
const int16_t DP_0_05KG_SM2         = DIFF_ADC(0.05);          // разница в 0.05 кг/см2 (A5)
const TickType_t T_PRES_UP_MIN      = 2000;                    // минимальное и максимальное время подкачки (В1)
const TickType_t T_PRES_UP_MAX      = 120000;                  // от 2 с до 2 мин
const TickType_t T_PRES_DOWN_MIN    = 2000;                    // минимальное и максимальное время спуска (В2)
const TickType_t T_PRES_DOWN_MAX    = 45000;                   // от 14 с до 45 с
const int16_t DP_0_3KG_SM2          = DIFF_ADC(0.3);           // разница в 0.3 кг/см2  (Г0)
const int16_t DP_0_15KG_SM2         = DIFF_ADC(0.15);          // разница в 0.15 кг/см2 (Г0)
const int16_t DP_0_5KG_SM2          = DIFF_ADC(0.5);           // разница в 0.5 кг/см2 (Д0)
const int16_t DP_1KG_SM2            = DIFF_ADC(1);             // разница в 1 кг/см2 (Д0)

const TickType_t P_UP_QUANT         = 2000;                    // квант времени подкачки
const TickType_t P_DOWN_QUANT       = 10;                      //??? // квант времени спуска
const int16_t P_COMPRESSOR          = PRESS_ADC(6);            //??? давление в компрессоре (6 атм)
const TickType_t T_PAUSE            = 500;                     // время паузы

const int16_t VDAT_0                = PRESS_ADC(1);            //Амтосферное давление по паспорту TODO: Установить
