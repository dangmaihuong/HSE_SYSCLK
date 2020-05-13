/*
 * it.c
 *
 *  Created on: May 12, 2020
 *      Author: dangm
 */

#include "main.h"


void SysTick_Handler (void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


