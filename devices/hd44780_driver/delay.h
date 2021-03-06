/**
 *	Pretty accurate delay functions with SysTick or any other timer
 *
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@link		http://stm32f4-discovery.com/2014/04/library-03-stm32f429-discovery-system-clock-and-pretty-precise-delay-library/
 *	@version 	v2.1
 *	@ide		Keil uVision
 *	@license	GNU GPL v3
 *
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 *
 * ------!!!!!!!!!!!------
 *	If you are using GCC (sucks) compiler, then your delay is probably totally inaccurate.
 *	USE TIMER FOR DELAY, otherwise your delay will not be accurate
 *
 *	Or use ARM compiler!
 * ------!!!!!!!!!!!------
 *
 * Version 2.1
 *	- GCC compiler fixes
 *	- Still prefer that you use TIM for delay if you are working with ARM-GCC compiler
 *
 * Version 2.0
 *	- November 28, 2014
 *	- Delay library has been totally rewritten. Because Systick is designed to be used
 *	  in RTOS, it is not compatible to use it at the 2 places at the same time.
 *	  For that purpose, library has been rewritten.
 *	- Read full documentation below
 *
 * As of version 2.0 you have now to possible ways to make a delay.
 * The first (and default) is Systick timer. It makes interrupts every 1ms.
 * If you want delay in "us" accuracy, there is simple pooling (variable) mode.
 *
 * The second (better) options is to use one of timers on F4xx MCU.
 * Timer also makes an interrupts every 1ms (for count time) instead of 1us as it was before.
 * For "us" delay, timer's counter is used to count ticks. It makes a new tick each "us".
 * Not all MCUs have all possible timers, so this lib has been designed that you select your own.
 *
 * By default, Systick timer is used for delay. If you want your custom timer,
 * open defines.h file, add lines below and edit for your needs:
 *
 *	//Select custom timer for delay, here is TIM2 selected.
 *	//If you want custom TIMx, just replace number "2" for your TIM's number.
 *	#define  DELAY_TIM				TIM2
 *	#define  DELAY_TIM_IRQ			TIM2_IRQn
 *	#define  DELAY_TIM_IRQ_HANDLER	TIM2_IRQHandler
 *
 * With this setting (using custom timer) you have better accuracy in "us" delay.
 * Also, you have to know, that if you want to use timer for delay, you have to include additional files:
 *
 *	- CMSIS:
 *		- STM32F4xx TIM
 *		- MISC
 *	- TM:
 *		TM TIMER PROPERTIES
 *
 * Delay functions (Delay, Delayms) are now Inline functions.
 * This allows faster execution and more accurate delay.
 *
 * If you are working with Keil uVision and you are using Systick for delay,
 * then set KEIL_IDE define in options for project:
 *	- Open "Options for target"
 *	- Tab "C/C++"
 *	- Under "Define" add "KEIL_IDE", without quotes
 */
#ifndef  DELAY_H
#define  DELAY_H 210
/**
 * Library dependencies
 * - STM32F4xx
 * - STM32F4xx RCC
 * - STM32F4xx TIM 			(Only if you want to use TIMx for delay instead of Systick)
 * - MISC         			(Only if you want to use TIMx for delay instead of Systick)
 * - TM TIMER PROPERTIES	(Only if you want to use TIMx for delay instead of Systick)
 * - defines.h
 */
/**
 * Includes
 */

#define  DELAY_TIM				TIM2
#define  DELAY_TIM_IRQ			TIM2_IRQn
#define  DELAY_TIM_IRQ_HANDLER	TIM2_IRQHandler

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
/* If user selectable timer is selected for delay */
#if defined( DELAY_TIM)
#include "misc.h"
#include "stm32f4xx_tim.h"
#include "timer_properties.h"
#endif



/**
 * This variable can be used in main
 * It is automatically increased every time systick make an interrupt
 */
extern __IO uint32_t  Time;
extern __IO uint32_t  Time2;
extern __IO uint32_t mult;

/**
 * Delay for specific amount of microseconds
 *
 * Parameters:
 * 	- uint32_t micros:
 *		Time in microseconds for delay
 */
static inline void Delay(uint32_t micros) {
#if defined( DELAY_TIM)
	volatile uint32_t timer =  DELAY_TIM->CNT;

	do {
		/* Count timer ticks */
		while (( DELAY_TIM->CNT - timer) == 0);

		/* Increase timer */
		timer =  DELAY_TIM->CNT;

//		volatile uint32_t counter=10;
//		while(counter) counter--;

		/* Decrease microseconds */
	} while (--micros);
#else
	uint32_t amicros;

	/* Multiply micro seconds */
	amicros = (micros) * (mult);

	#ifdef __GNUC__
		if (SystemCoreClock == 180000000 || SystemCoreClock == 100000000) {
			amicros -= mult;
		}
	#endif

	/* If clock is 100MHz, then add additional multiplier */
	/* 100/3 = 33.3 = 33 and delay wouldn't be so accurate */
	#if defined(STM32F411xE)
	amicros += mult;
	#endif

	/* While loop */
	while (amicros--);
#endif /*  DELAY_TIM */
}

/**
 * Delay for specific amount of milliseconds
 *
 * Parameters:
 * 	- uint32_t millis:
 *		Time in milliseconds for delay
 */
static inline void Delayms(uint32_t millis) {
	volatile uint32_t timer =  Time;

	/* Wait for timer to count milliseconds */
	while (( Time - timer) < millis);
}

/**
 * Initialize timer settings for delay
 * This function will initialize Systick or user timer, according to settings
 *
 */
void  DELAY_Init(void);

/**
 * Get the  Time variable value
 *
 * Current time in milliseconds is returned
 */
#define  DELAY_Time()					( Time)

/**
 * Set value for  Time variable
 *
 * Parameters:
 *	- time:
 *		Time in milliseconds
 */
#define  DELAY_SetTime(time)			( Time = (time))

/**
 * Re-enable delay timer. It has to be configured before with  DELAY_Init();
 *
 * This function enables delay timer. It can be systick or user selectable timer.
 *
 */
void  DELAY_EnableDelayTimer(void);

/**
 * Disable delay timer.
 *
 * This function disables delay timer. It can be systick or user selectable timer.
 */
void  DELAY_DisableDelayTimer(void);

/**
 * Get the  Time2 variable value
 *
 * Current time in milliseconds is returned
 *
 * This is not meant for public use
 */
#define  DELAY_Time2()				( Time2)

/**
 * Set value for  Time variable
 *
 * Parameters:
 *	- time:
 *		Time in milliseconds
 *
 * This is not meant for public use
 */
#define  DELAY_SetTime2(time)			( Time2 = (time))

#endif
