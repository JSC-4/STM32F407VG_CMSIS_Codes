/* 
 * This program is a basic use CRC on the STM32F407VG board. 
 * The datasheets used for this is are the AN4187 and RM0090.
 *
 * ---- Configuration steps ----
 * Enable CRC clock
 * Reset CRC
 *
 * Write data into CRC data register
 * Read CRC data register for calculated CRC value
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 11/12/20
 *
 */

#include "stm32f4xx.h"                  // Device header

#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"

#define DEBUG_UART	USART2
#define delay				for(int i=0; i<50000; i++)

void UART2_Enable(void);
static void printMsg (char *msg, ...);	// Print message function

void crc_init(void);

int main(void)
{
	int test_byte = 0xFF;
	
	UART2_Enable();
	crc_init();
	
	while (1)
	{
		
		CRC->CR |= CRC_CR_RESET;	// Reset CRC
		CRC->DR = test_byte;		// Write data into the CRC data register
		printMsg("The test byte CRC value is: %x \n", CRC->DR);		// Print calculated CRC value from data register
		delay;
		
	}
	
}

void crc_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;		// Enable CRC	
	CRC->CR |= CRC_CR_RESET;	// Reset CRC
	
}

void UART2_Enable(void)
{
	// Enable USART2 and Port A for TX (PA2) and RX (PA3) pins 
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->AFR[0] |= (0x7 << 8); 								// Set PA2 and PA3 alternate functions for USART2
	GPIOA->MODER |= GPIO_MODER_MODE2_1;	// Configure PA2 and PA3 as alternate functions 
	
	USART2->BRR = 0x0683;		// Baud rate (16Mhz/2*9600)	
	USART2->CR1 |= USART_CR1_TE | USART_CR1_UE; // Enable Rx, Tx and USART2
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
