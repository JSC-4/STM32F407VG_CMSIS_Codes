/* 
 * This program is a way to print values to a terminal using the UART for the STM32F407VG board.
 * The datasheet used for this is the RM0090 and STM32F407xx datasheet
 *
 * ---- Configuration steps ----
 * First configure the RCC register for USART and GPIO
 * Configure Alternate function registers and mode register
 * Configure Baud rate
 * Enable USART Tx and enable pins in CR1
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 22/11/20
 *
 */
 
 
#include "stm32f407xx.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"

#define DEBUG_UART	USART2
#define delay				for(int i=0; i<50000; i++)

// Print message function
static void printMsg (char *msg, ...);


int main(void)
{
	// Enable USART2 and Port A for TX (PA2) and RX (PA3) pins 
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->AFR[0] |= (0x7 << 8); 								// Set PA2 and PA3 alternate functions for USART2
	GPIOA->MODER |= GPIO_MODER_MODE2_1;	// Configure PA2 and PA3 as alternate functions 
	
	USART2->BRR = 0x0683;		// Baud rate (16Mhz/2*9600)	
	USART2->CR1 |= USART_CR1_TE | USART_CR1_UE; // Enable Rx, Tx and USART2
	
	while (1)
	{
		printMsg("I am %d years old\n", 24);
		delay;
	}
}

static void printMsg (char *msg, ...)
{
	char buff[80];
	#ifdef DEBUG_UART		// not needed
	
		va_list args;
		va_start(args, msg);
		vsprintf(buff, msg, args);
	
		for (int i=0; i<strlen(buff); i++)
		{
			USART2->DR = buff[i];
			while(!(USART2->SR & USART_SR_TXE));
		}
	
	#endif
}
