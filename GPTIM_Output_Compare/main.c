/* 
 * This program is a basic use of general purpose timer 4 to blink an LED
 * on the STM32F407VG board, using output compare. The datasheet used for 
 * this is the Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 * 
 * ---- Configuration steps ----
 * Configure the RCC register for GPIO
 * Configure mode as alternata function for timer
 * Set in the alternate function register the pins for timer (check AFR table)
 * Configure the RCC register for the timer 
 * Configure the timer prescale and autoload registers
 * Configure the Capture Compare Mode Register for toggle mode
 * Set the compare value to zero in the CCR1 register.
 * Enable the capture/compare 1 output enable bit
 * Clear timer count register
 * Enable the timer
 * 
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 25/11/20
 *
 * NOTE(1): To work out the value to count to do (fclk/prescale * Auto Reload Register)
 *  Example to create a 1hz counter prescaler = 1600 - 1, ARR = 10000 -1 (16MHz/(1600*10000))
 * NOTE(2): Read the datasheet for alternate functions as different boards are mapped differently
 */
 
#include "stm32f407xx.h"

int main()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;		// Enable the clock for port D
	GPIOD-> MODER |= GPIO_MODER_MODE12_1;		// Set PD12 to alternate function 
	GPIOD->AFR[1] |= (1 << 17);				// Set PD12 alternate function to timer 4 channel 1
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;		// Enable the clock for timer 4
	TIM4->PSC = 1600 - 1;				// Divide fclk to 10,000
	TIM4->ARR = 10000 - 1;			// Divide (fclk/psc) by 10,000 to create 1Hz signal
	TIM4->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1;		// 011: Toggle - OC1REF toggles when TIMx_CNT=TIMx_CCR1.
	TIM4->CCR1 = 0;			// Set compare value?
	TIM4->CCER |= TIM_CCER_CC1E;	// Enable the capture/compare 1 output enable
	TIM4->CNT = 0;					// Clear timer register
	TIM4->CR1 |= TIM_CR1_CEN;		// Enable timer 4
	
	while (1)
	{
		
	}
}
