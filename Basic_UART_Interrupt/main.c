/*
 * This program is a basic echo sketch (interrupt) using USART2 for the STM32F407VG board.
 * The datasheet used for this is the RM0090 and STM32F407xx datasheet
 *
 * ---- Configuration steps ----
 * First configure the RCC register for USART and GPIO
 * Configure Alternate function registers and mode register
 * Enable Rx and Tx interrupt flags
 * Configure Baud rate
 * Enable USART Rx, Tx and enable pins in CR1
 * Enable USART interrupt by using NVIC_EnableIRQ *CHECK NOTES*
 * Write interrupt function which checks status register flags to 
 * 	see when data or transmit register are empty
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 22/11/20
 *
 * NOTE: Moving line: (NVIC_EnableIRQ(USART2_IRQn)) above the USART2 enable does not work!
 */
 
#include "stm32f407xx.h"

// Basic delay function
void delayMs (int delay);
void USART2_IRQHandler(void);

int main(void)
{
	// Enable USART2 and Port A for TX (PA2) and RX (PA3) pins 
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->AFR[0] |= (0x7 << 8) | (0x7 << 12); 								// Set PA2 and PA3 alternate functions for USART2
	GPIOA->MODER |= GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1;	// Configure PA2 and PA3 as alternate functions 

	USART2->CR1 |= USART_CR1_RXNEIE |USART_CR1_TXEIE;		// Enable RXNE and TXE interrupts 
	
	USART2->BRR = 0x0683;		// Baud rate (16Mhz/2*9600)	
	USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE; // Enable Rx, Tx and USART2
	NVIC_EnableIRQ(USART2_IRQn);		// Enable interrupt for USART2 on NVIC
	
	while (1)
	{

	}
}
void USART2_IRQHandler(void)
{
		// Check to see if RXNE caused the interrupt
		if (USART2->SR & USART_SR_RXNE) // Check if receive read data register ready to be read empty
		{
			char temp = USART2->DR;		// Read data regiser
			USART2->DR = temp;				// write data back to data register
			//delayMs(100);
			while (!(USART2->SR & USART_SR_TC)){}		// Wait for transmission to be complete
		}
//		// Check to see if TXE caused the interrupt
//		if (USART2->SR & USART_SR_TXE) // Check if receive read data register ready to be read empty
//		{
//			// Empty as it's not needed in this program
//		}
}

void delayMs (int delay)
{
	int i;
	for (;delay>0;delay--)
	{
		for (i=0; i<3195; i++);
	}
	
	
}
