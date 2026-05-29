#include "stm32f10x.h"
#include "Delay.h"
#include "HX711.h"

//引脚定义，定义HX711与STM32的接线
#define HX711_DT_PORT              GPIOB
#define HX711_DT_PIN               GPIO_Pin_0
#define HX711_SCK_PORT             GPIOB
#define HX711_SCK_PIN              GPIO_Pin_1

//去皮值
static int32_t HX711_Offset = 0;

//控制SCK引脚输入高/低电平（时钟输入）
static void HX711_SCK_Write(uint8_t BitValue)
{
	GPIO_WriteBit(HX711_SCK_PORT, HX711_SCK_PIN, (BitAction)BitValue);
}

//读取DOUT引脚（数据输出）
static uint8_t HX711_DT_Read(void)
{
	return GPIO_ReadInputDataBit(HX711_DT_PORT, HX711_DT_PIN);
}

//初始化HX711
void HX711_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = HX711_DT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HX711_DT_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin = HX711_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HX711_SCK_PORT, &GPIO_InitStructure);
	
	//默认把SCK拉低
	HX711_SCK_Write(0);
}

//读取HX711原始数据
int32_t HX711_ReadRaw(void)
{
	uint8_t i;
	uint32_t data = 0;
	
	/*while(HX711_DT_Read() == 1)
	{
		//一直等待
	}
	DT = 1 数据还没准备好
	DT = 0 数据已经准备好了*/
	while (HX711_DT_Read());
	
	for (i = 0; i < 24; i++)
	{
		HX711_SCK_Write(1);
		Delay_us(1);
		data = data << 1;
		HX711_SCK_Write(0);
		Delay_us(1);
		
		if (HX711_DT_Read())
		{
			data++;
		}
	}
	
	/* 25th pulse: channel A, gain 128. */
	HX711_SCK_Write(1);
	Delay_us(1);
	HX711_SCK_Write(0);
	Delay_us(1);

	//24位有符号数扩展成32位
	if (data & 0x800000)
	{
		data |= 0xFF000000;
	}
	
	return (int32_t)data;
}

//多次读取求平均：减少数据抖动
int32_t HX711_ReadAverage(uint8_t samples)
{
	uint8_t i;
	int32_t sum = 0;
	
	for (i = 0; i < samples; i++)
	{
		sum += HX711_ReadRaw();
	}
	
	return sum / samples;
}

//去皮函数
void HX711_Tare(void)
{
	HX711_Offset = HX711_ReadAverage(HX711_TARE_SAMPLES);
}

//获取重量，单位为克
int32_t HX711_GetWeightGram(void)
{
	int32_t value;
	
	value = HX711_ReadAverage(HX711_READ_SAMPLES) - HX711_Offset;
	return value / HX711_COUNTS_PER_GRAM;
}
