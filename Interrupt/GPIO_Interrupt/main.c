/* 
 * This program is an example of using the gpio external interrupt.
 * The external interrupt is connected to the user switch, which
 * lights an LED on the falling edge. This example is based on the 
 * STM32F407VG board. The datasheet used for this is the 
 * Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 *
 * ---- Configuration steps ----
 * Disable all interrupts
 * Configure the RCC register for GPIO and system configuration
 * Configure mode as output for LED and input for switch (optional)
 * Set interrupt pin for interrupt
 * Remove interrupt mask, by setting the bit in the IMR register
 * Set trigger event as falling or rising
 * Enable external interrupt x
 * Enable all interrupts
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 01/12/20
 *
 * NOTE(1): Interrupts are masked initially to reduce noise? (research this).
 * NOTE(2): Some interrupts are grouped together by name such as EXTI15_10_IRQn.
 * 		Read the EXTI-PR register in the handler function, to see what interrupt was set.
 */
 
#include "stm32f407xx.h"

void EXTI0_IRQHandler(void);
void delayms (int delay);
	
int main()
{

	__disable_irq();		// Disable all interrupts
	
	// Turn on clock for GPIOA (switch), GPIOD (LED) and system configuration controller
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	GPIOD->MODER |= GPIO_MODER_MODE12_0;	// Set PD12 as output for LED
	GPIOA->MODER &= ~GPIO_MODER_MODE0; //Set PA0 as input for switch (not needed as default mode is input)
	
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; // Set PA0 as an external interrupt
	EXTI->IMR |= EXTI_EMR_EM0;		// Enable the interrupt by turning off the mask
	EXTI->FTSR |= EXTI_FTSR_TR0;	// Set trigger event to rising
	
	NVIC_EnableIRQ(EXTI0_IRQn);		// Enable external intrrupt 0
	
	__enable_irq();		// Enable all interrupts
	
	while (1)
	{

	}

}

// Function for external interrupt 0
void EXTI0_IRQHandler(void)
{
	GPIOD->BSRR = (1<<12);
	delayms(300);
	GPIOD->BSRR = (1<<(12+16));
	delayms(300);
	GPIOD->BSRR = (1<<12);
	delayms(300);
	GPIOD->BSRR = (1<<(12+16));
	delayms(300);
	
	EXTI->PR |= EXTI_PR_PR0;		// Clear interrupt flag

}

void delayms (int delay)
	{
		SysTick->LOAD = 16000 - 1;			// Start value to load into the SysTick Reload Value Register <CHECK NOTES>
		SysTick->VAL = 0;									// Clear the Current Value Register
		SysTick->CTRL |= (0x5 << 0);			// Enable the SysTick Control and Status Register and set clksource to processor clock

		for (int i = 0; i < delay; i++)
		{
			while (!(SysTick->CTRL & (1 << 16))) // Check to see if COUNTFLAG was set
				;
		}
	}
