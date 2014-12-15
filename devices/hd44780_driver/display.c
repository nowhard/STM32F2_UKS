#include "display.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "watchdog.h"
#include "uks.h"
#include "hd44780.h"
#include "ADS1120.h"
#include "stdio.h"

extern struct ADS1120_result ADS1120_res;
extern struct uks uks_channels;

void Display_Task(void *pvParameters );

void Display_Init(void)
{
	HD44780_Init(20,4);
	xTaskCreate(Display_Task,(signed char*)"DISPLAY_TASK",128,NULL, tskIDLE_PRIORITY + 1, NULL);
   // task_watches[DISPLAY_TASK].task_status=TASK_IDLE;
}

static volatile uint16_t display_init_counter=0;
#define DISPLAY_INIT_PERIOD		3

void Display_Task(void *pvParameters )
{
	static volatile uint8_t str_buf[32];
	while(1)
	{
		vTaskDelay(300);
		if(display_init_counter>=DISPLAY_INIT_PERIOD)
		{
			HD44780_Reinit(20,4);
			display_init_counter=0;
		}
		else
		{
			display_init_counter++;
		}


		UKS_Sort_Channels(&uks_channels,DRYING_CHANNELS_NUM);

		uint8_t i=0;
		for(i=0;i<4/*DRYING_CHANNELS_NUM*/;i++)
		{
			sprintf(str_buf,"%1d",uks_channels.drying_channel_sort_list[i]->number);
			HD44780_Puts(0,i,str_buf);
		}

//		sprintf(str_buf,"VAL=%09d       ",ADS1120_res.result);
//		HD44780_Puts(0,0,str_buf);
//
//		float temp=PT100_Code_To_Temperature(ADS1120_res.result);
//		if(temp>=0)
//		{
//			sprintf(str_buf,"TEMP= %3d.%01d         ",(uint16_t)temp,(uint16_t)(temp*10)%10);
//		}
//		else
//		{
//			sprintf(str_buf,"TEMP=-%3d.%01d         ",(uint16_t)(-temp),(uint16_t)(((-temp)*10))%10);
//		}
//
//
//		HD44780_Puts(0,1,str_buf);
//
//		temp=uks_channels.drying_channel_list[0].temperature;
//		if(temp>=0)
//		{
//			sprintf(str_buf,"TEMP= %3d.%01d         ",(uint16_t)temp,(uint16_t)(temp*10)%10);
//		}
//		else
//		{
//			sprintf(str_buf,"TEMP=-%3d.%01d         ",(uint16_t)(-temp),(uint16_t)(((-temp)*10))%10);
//		}
//
//		HD44780_Puts(0,2,str_buf);
//
//		temp=uks_channels.drying_channel_list[0].temperature_queue[uks_channels.drying_channel_list[0].temperature_queue_counter]-uks_channels.drying_channel_list[0].temperature_queue[(uks_channels.drying_channel_list[0].temperature_queue_counter-10)&(TEMPERATURE_QUEUE_LEN-1)];
//
//		if(temp>=0)
//		{
//			sprintf(str_buf,"TEMP= %3d.%01d         ",(uint16_t)temp,(uint16_t)(temp*10)%10);
//		}
//		else
//		{
//			sprintf(str_buf,"TEMP=-%3d.%01d         ",(uint16_t)(-temp),(uint16_t)(((-temp)*10))%10);
//		}
//
//		HD44780_Puts(0,3,str_buf);

	}
}
