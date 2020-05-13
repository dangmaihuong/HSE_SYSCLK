/*
 * main.c
 * Author: Huong Dang
 * Date: May 12nd, 2020
 * purpose: Learning using peripheral UART
 * Content: Send character over UART to MCU. MCU convert all lower case
 * into upper case and send it back to user
 *
 */

#include <string.h>
#include "main.h"
#include <stdio.h>

void UART2_Init(void);
void Error_handler(void);

UART_HandleTypeDef huart2;


int main(void)
{

	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;

	char msg[100];

	HAL_Init();
	UART2_Init();

	memset(&osc_init, 0, sizeof(osc_init));

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_BYPASS; //HSE is off after power up or after reset by default.

	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK){ //after this line, HSE will be turn on and ready to use
		Error_handler();
	}

	//If use higher and higher HCLK, the power consumption of application shoots up
	//using AHB prescaler in order to reduce the HCLK.
	//APB bus is a peripheral bus which is very very slow compared to the high speed bus that is the AHB
	//So we can not give HCLK directly to APB bus, because the maximum speed of the APB1 is 45MHz, APB2 is 90MHz
	//using APB prescaler in order to reduce HCLK once again.

	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

	//to corectly read data from flash memory, the number of wait sate must be correctly programmed in the Flash access control register (FLASH_ACR) according to frequency of HCLK and supply voltage of device. (this is meaning of latency)
	//so, if HCLK is very high, then flash memory not cooperate with HCLK
	//in our case, HCLK = 4MHz so wait state = 0 (in manual reference table)

	if (HAL_RCC_ClockConfig(&clk_init, 0) != HAL_OK){
		Error_handler();
	}


	//-------------------AFTER THIS LINE SYSCLK IS SOURCED BY HSE--------------------



	__HAL_RCC_HSI_DISABLE(); //save some current


	//REDO THE SYSTICK CONFIGURATION

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	//baudrate is based on APB clock, now it is changed into 2MHz, so called it once again
	UART2_Init();

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SYSCLK: %ld\r\n", HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "HCLK: %ld\r\n", HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "APB1: %ld\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "APB2: %ld\r\n", HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	while(1);

	return 0;
}


void UART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if ( HAL_UART_Init(&huart2) != HAL_OK )
	{
		//There is a problem
		Error_handler();
	}


}



void Error_handler(void)
{
	while(1);
}

