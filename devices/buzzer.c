#include "buzzer.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include <misc.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "watchdog.h"
#include "uks.h"

struct buzzer buzz;
extern struct task_watch task_watches[];
xTaskHandle xBuzzer_Handle;

void Buzzer_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure port -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);


    GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);

    xTaskCreate(Buzzer_Task,(signed char*)"BUZZER",64,NULL, tskIDLE_PRIORITY + 1, &xBuzzer_Handle);
//    task_watches[BUZZER_TASK].task_status=TASK_IDLE;
}
void Buzzer_Task(void *pvParameters )
{
	while(1)
	{
		if(buzz.buzzer_enable==BUZZER_ON)
		{
			switch(buzz.buzzer_effect)
			{
				case BUZZER_EFFECT_0:
				{
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
					vTaskDelay(500);
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
					vTaskDelay(500);
				}
				break;

				case BUZZER_EFFECT_1:
				{
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
					vTaskDelay(100);
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
					vTaskDelay(200);
				}
				break;

				case BUZZER_EFFECT_2:
				{
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
					vTaskDelay(100);
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
					vTaskDelay(200);
				}
				break;

				case BUZZER_EFFECT_3:
				{
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
					vTaskDelay(100);
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
					vTaskDelay(200);
				}
				break;

				case BUZZER_EFFECT_4:
				{
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
					vTaskDelay(100);
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
					vTaskDelay(200);
				}
				break;

				case BUZZER_EFFECT_5:
				{
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
					vTaskDelay(100);
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
					vTaskDelay(200);
				}
				break;

				case BUZZER_EFFECT_6:
				{
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
					vTaskDelay(100);
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
					vTaskDelay(200);
				}
				break;

				case BUZZER_EFFECT_7:
				{
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
					vTaskDelay(100);
					GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
					vTaskDelay(200);
				}
				break;

				default:
				{
					vTaskDelay(500);
				}
			}
		}
		task_watches[BUZZER_TASK].counter++;
	}
}


void Buzzer_Set_Buzz(uint8_t effect, uint8_t enable)
{
	if(enable&0x1)
	{
		if(buzz.buzzer_enable==BUZZER_OFF)
		{
			vTaskResume(xBuzzer_Handle);
			buzz.buzzer_enable=BUZZER_ON;
		}
		task_watches[BUZZER_TASK].task_status=TASK_ACTIVE;
	}
	else
	{
		if(buzz.buzzer_enable==BUZZER_ON)
		{
			vTaskSuspend (xBuzzer_Handle);
			buzz.buzzer_enable=BUZZER_OFF;
		}
		task_watches[BUZZER_TASK].task_status=TASK_IDLE;
		GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
	}
	buzz.buzzer_effect=effect&0x7;
}
