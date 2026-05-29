#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "HX711.h"

static void OLED_ShowWeight(int32_t weight)
{
	uint32_t absWeight;
	
	if (weight < 0)
	{
		absWeight = (uint32_t)(-weight);
		OLED_ShowChar(2, 9, '-');
	}
	else
	{
		absWeight = (uint32_t)weight;
		OLED_ShowChar(2, 9, ' ');
	}
	
	OLED_ShowNum(2, 10, absWeight, 5);
	OLED_ShowString(2, 15, "g");
}

int main(void)
{
	int32_t weight;
	
	OLED_Init();
	HX711_Init();
	
	OLED_ShowString(1, 1, "ElectronicScale");
	OLED_ShowString(2, 1, "Taring...");
	OLED_ShowString(3, 1, "Keep pan empty");
	Delay_ms(2000);
	HX711_Tare();
	
	OLED_Clear();
	OLED_ShowString(1, 1, "HX711 Scale");
	OLED_ShowString(2, 1, "Weight:");
	OLED_ShowString(3, 1, "Keep pan empty");
	
	while (1)
	{
		weight = HX711_GetWeightGram();
		OLED_ShowWeight(weight);
		Delay_ms(200);
	}
}
