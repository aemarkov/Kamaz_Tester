/*
Реализация алгоритмов
*/

#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include <API\api.h>

#define WHEELS_COUNT            6

/* Формулы для перевода давления
если в Вольтах         N = V / 5.1 * 4096
если в атм (кг/см2)    N = (p + 1) * 321 + 482
если разница давлений  N = (dp / 1) * 321
*/

#define EPS_EQUAL               200                     // погрешность возвращения давления к норме (А1)
#define EPS_RISING              30                      // погрешность увеличение давления (А1)
#define NORMAL_PRESSURE         4                       // нормальное давление, 4 кг/см2 (A1)
#define V1_1                    883                     // верхняя и нижняя границы атмосферного давления (А2)
#define V0_4                    321                     // 0.4 - 1.1В        	   
#define P_6KG_SM2               2729                    // 6 кг/см2  (А3)
#define DP_0_1KG_SM2            32                      // разница в 0.1 кг/см2(A3, A4, Г0)
#define DP_0_2KG_SM2            64                      // разница в 0.2 кг/см2(A4, В0)
#define DP_0_05KG_SM2           16                      // разница в 0.05 кг/см2 (A5)
#define T_PRES_UP_MIN           2000                    // минимальное и максимальное время подкачки (В1)
#define T_PRES_UP_MAX           120000                  // от 2 с до 2 мин
#define T_PRES_DOWN_MIN         2000                    // минимальное и максимальное время спуска (В2)
#define T_PRES_DOWN_MAX         45000                   // от 14 с до 45 с
#define DP_0_3KG_SM2            96                      // разница в 0.3 кг/см2  (Г0)
#define DP_0_15KG_SM2           48                      // разница в 0.15 кг/см2 (Г0)
#define DP_0_5KG_SM2            160                     // разница в 0.5 кг/см2 (Д0)
#define DP_1KG_SM2              321                     // разница в 1 кг/см2 (Д0)

#define P_UP_QUANT              2000                    // квант времени подкачки
#define P_DOWN_QUANT            10                      //??? // квант времени спуска
#define P_COMPRESSOR            2729                    //??? давление в компрессоре (6 атм)
#define T_PAUSE                 500                     // время паузы

#define VDAT_0                  1                       //Амтосферное давление по паспорту TODO: Установить

typedef enum
{
	ALG_OK,
	ALG_ERROR,
	ALG_CRITICAL_ERROR
} AlgResult;



//Самодиагностика  систмеы при включении
AlgResult A0(void);

//Проверка работоспособности клапанов управления входным давлением К8 и К9
AlgResult A1(void);

//Измерение атмосферного давления Р0 и самоадаптация к датчику давления
AlgResult A2(void);

//Измерение начального давления в питающей магистрали
AlgResult A3(void);

//Проверка герметичности БУД на избыточное давление
AlgResult A4(void);

//Проверка герметичности БУД на вакуум
AlgResult A5(void);

//Измерение текущего давления воздуха в шинах
void A6(uint8_t wheels);

//Работа системы в дежурном режиме
void B0(int16_t requiredWheelsPressure);

//Повышение давления воздуха в колесе (-ах)
void B1(uint8_t wheels, uint16_t requiredPressure);

//Снижение давления воздуха в колесе (-ах)
void B2(uint8_t wheels, uint16_t requiredPressure);

//Определение повреждения колеса
void G0(void);

void D0(uint8_t damagedWheels);

#endif
