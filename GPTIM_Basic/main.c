/* 
 * This program is a basic use of general purpose timer 2 to blink an LED
 * on the STM32F407VG board. The datasheet used for this is the 
 * Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 *
 * ---- Configuration steps ----
 * Configure the RCC register for GPIO
 * Configure mode as output for LED
 * Configure the RCC register for the timer 
 * Configure the timer prescale and autoload registers
 * Clear timer count register
 * Enable the timer
 * Check to see if timer flag has been set in the timer status register
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 24/11/20
 *
 * NOTE: To work out the value to count to do (fclk/prescale * Auto Reload Register)
 * 		 Example to create a 1hz counter prescaler = 1600 - 1, ARR = 10000 -1 (16MHz/(1600*10000))
 */
 
 #include "stm32f407xx.h"
 
int main()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;		// Turn on clock for LED 
	GPIOD->MODER |= GPIO_MODER_MODE13_0;		// Set LED as output (PD13)
    
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;			// Turn on clock for timer 2

	TIM2->PSC = 1600 - 1;				// 16Mhz / Prescale register
	TIM2->ARR = 10000 - 1;			// Divde timer 2 again by 10,000 to get 1 (16MHz/prescale * Auto Reload Register)
	TIM2->CNT = 0;							// Clear timer register
	TIM2->CR1 = TIM_CR1_CEN;		// Enable timer 2

	while (1)
	{
		if (TIM2->SR & TIM_SR_UIF)		// Check if timer flag has been set
		{
			TIM2->SR &= ~TIM_SR_UIF; 		// Clear timer flag
			GPIOD->ODR ^= (1 << 13);		// Flip LED
		}
	}	 
}
