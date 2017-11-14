/*
Реализация алгоритмов
*/

#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__

#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include <API\api.h>

#include "alg_consts.h"

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
