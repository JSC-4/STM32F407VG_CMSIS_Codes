/*
 * This program is a basic use of general purpose timer 4 to blink an LED
 * on the STM32F407VG board, using the interrupt. The datasheet used for 
 * this is the Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 * 
 * ---- Configuration steps ----
 * Disable all interrupts
 * Configure the RCC register for GPIO
 * Configure mode as output for GPIO pin
 * Configure the RCC register for the timer 
 * Configure the timer prescale and autoload registers
 * Set URS bit in the CR1 register
 * Set UIE bit in the DIER register
 * Set UG bit in the EGR register
 * Enable the timer
 * Enable timer in the NVIC
 * Enable all interrupts
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 03/12/20
 *
 * NOTE(1): To work out the value to count to do (fclk/prescale * Auto Reload Register)
 *  Example to create a 1hz counter prescaler = 1600 - 1, ARR = 10000 -1 (16MHz/(1600*10000))
 * NOTE(2): Read the datasheet for alternate functions as different boards are mapped differently
 */
 
#include "stm32f4xx.h"                  // Device header

int main()
{
	__disable_irq();		// Disable all interrupts
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;		// Turn on clock for GPIOD
	GPIOD->MODER |= GPIO_MODER_MODE13_0;		// Set PD13 to output mode
		
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;		// Turn on clock for timer 4
	TIM4->PSC = 16000 - 1;		// Divide fclk to 1000
	TIM4->ARR = 1000 - 1;		// Divide clock again to get a 1Hz pulse
	TIM4->CR1 |= TIM_CR1_URS;		// Only cause interrupt when underflow/overflow
	TIM4->DIER |= TIM_DIER_UIE;		// Enable update interrupt bit
	TIM4->EGR |= TIM_EGR_UG;		// Enable update generation
	TIM4->CR1 |= TIM_CR1_CEN;		// Enable timer 4
	
	NVIC_EnableIRQ(TIM4_IRQn);		// Enable timer 4 intrrupt

	__enable_irq();			// Enable all interrupts
	
	while (1)
	{
		
	}
}

void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF;		// Clear interrupt flag
	GPIOD->ODR ^= GPIO_ODR_OD13;	
}
