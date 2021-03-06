#include "adc.h"
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_adc.h>


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdio.h"
#include "string.h"
#include "hd44780.h"
#include "uks.h"

#include "watchdog.h"


struct adc_lm35_channels adc_lm35_chnl;
extern struct uks uks_channels;

xSemaphoreHandle xMeasure_LM35_Semaphore;

extern struct task_watch task_watches[];

static void ADC_Task(void *pvParameters);

void ADC_Channel_Init(void)
{
	   ADC_InitTypeDef ADC_InitStructure;
	   ADC_StructInit(&ADC_InitStructure);

	   ADC_CommonInitTypeDef ADC_CommonInitStructure;
	   ADC_CommonStructInit(&ADC_CommonInitStructure);

	   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	   GPIO_InitTypeDef gpio;
	   GPIO_StructInit(&gpio);
	   gpio.GPIO_Mode = GPIO_Mode_AN;
	   gpio.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	   GPIO_Init(GPIOA, &gpio);

	   /* ��������� ������������ A��1 */
	   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	   /* ���������� ��������� ��� */
	   ADC_DeInit();
//
		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
		ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
		ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
		ADC_CommonInit(&ADC_CommonInitStructure);


	   ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	   ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	   ADC_InitStructure.ADC_NbrOfConversion=1;
	   ADC_Init(ADC1, &ADC_InitStructure);

	   ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_480Cycles);


	   /* �������� ���1 */
	   ADC_Cmd(ADC1, ENABLE);
	   vSemaphoreCreateBinary( xMeasure_LM35_Semaphore );
	   xTaskCreate(ADC_Task,(signed char*)"ADC",128,NULL, tskIDLE_PRIORITY + 1, NULL);
}

#define SWAP(A, B) { uint16_t t = A; A = B; B = t; }

void bubblesort(uint16_t *a, uint16_t n)
{
  uint16_t i, j;

  for (i = n - 1; i > 0; i--)
  {
    for (j = 0; j < i; j++)
    {
      if (a[j] > a[j + 1])
        SWAP( a[j], a[j + 1] );
    }
  }
}

#define NUM_CONV	16
#define MAX_ADC_CODE 	4095
#define MAX_ADC_VOLTAGE	3.3
#define LM35_V_FOR_C	0.01

static void ADC_Task(void *pvParameters)
{
		uint8_t i=0,j=0;
		task_watches[ADC_TASK].task_status=TASK_ACTIVE;
		while(1)
		{
			  for(i=0;i<ADC_FILTER_BUFFER_LEN;i++)
			  {
				   for(j=0;j<ADC_LM35_CHANNELS_NUM;j++)
				   {
					   ADC_RegularChannelConfig(ADC1, j, 1, ADC_SampleTime_480Cycles);
					   ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;
					   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
					   {
						   taskYIELD ();
					   }
					   adc_lm35_chnl.filter_buffer[j][i]=ADC1->DR;

				   }
			  }

			   for(j=0;j<ADC_LM35_CHANNELS_NUM;j++)
			   {
				   bubblesort(adc_lm35_chnl.filter_buffer[j],ADC_FILTER_BUFFER_LEN);
				   adc_lm35_chnl.channel[j]=((adc_lm35_chnl.filter_buffer[j][(ADC_FILTER_BUFFER_LEN>>1)-1]+adc_lm35_chnl.filter_buffer[j][ADC_FILTER_BUFFER_LEN>>1])>>1);

				   uks_channels.drying_channel_list[j].temperature_queue_counter++;
				   uks_channels.drying_channel_list[j].temperature_queue_counter&=(TEMPERATURE_QUEUE_LEN-1);
				   uks_channels.drying_channel_list[j].temperature=uks_channels.drying_channel_list[j].temperature_queue[uks_channels.drying_channel_list[j].temperature_queue_counter]=(float)adc_lm35_chnl.channel[j]/MAX_ADC_CODE*MAX_ADC_VOLTAGE/LM35_V_FOR_C;
			   }

			  xSemaphoreGive(xMeasure_LM35_Semaphore);
			  vTaskDelay(1000);
			  task_watches[ADC_TASK].counter++;
		}
}
