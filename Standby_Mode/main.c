/* 
 * This program is a basic use of using standby modeon the 
 * STM32F407VG board. The datasheet used for this is the 
 * Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 *
 * ---- Configuration steps ----
 * Enable the PWR control clock
 * Set SLEEPDEEP bit (from the cortex datasheet)
 * Select standby mode
 * Clear wake up flag
 * Enable wake up pin (check datasheet)
 * Enable Wait For Interrupt
 * 
 * Check if standby flag has been set to check if board was woken up
 * 	by either standby or power cycle
 *
 * NOTE(1): Comming out of wake up, is like resetting the board
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

void gotoSleep(void);
void UART2_Enable(void);
static void printMsg (char *msg, ...);	// Print message function


int main(void)
{
	UART2_Enable();
	
	// Check if standby flag was set
	if (PWR->CSR & PWR_CSR_SBF)
	{
		PWR->CR |= PWR_CR_CWUF | PWR_CR_CSBF; // Clear wake up and standby flag
		printMsg("I have waken from standby\n");
	}
	else
		printMsg("I have awaken from power cycle\n"); // PA0 was set high
		
	while (1)
	{
		printMsg("===GOING TO SLEEP===\n");
		gotoSleep();
		printMsg("===THIS SHOULD NOT RUN===\n");
	}
	
}

void gotoSleep(void)
{
	// Enable the PWR control clock
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	
	// Set SLEEPDEEP from the Cortex M4 System Control Register
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	
	// Select standby mode
	PWR->CR |= PWR_CR_PDDS;
	
	// Clear wake up flag
	PWR->CR |= PWR_CR_CWUF;
	
	// Enable wake up pin
	PWR->CSR |= PWR_CSR_EWUP;
	
	// Request Wait For Interrupt
	__WFI();	
	
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

