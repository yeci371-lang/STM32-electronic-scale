#ifndef __HX711_H
#define __HX711_H

#include "stm32f10x.h"

#define HX711_COUNTS_PER_GRAM      210 //校准系数
#define HX711_TARE_SAMPLES         50
#define HX711_READ_SAMPLES         10

void HX711_Init(void);
void HX711_Tare(void);
int32_t HX711_ReadRaw(void);
int32_t HX711_ReadAverage(uint8_t samples);
int32_t HX711_GetWeightGram(void);

#endif
