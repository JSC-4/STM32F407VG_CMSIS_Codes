/*
 * This program is a basic use of SysTick to blink an LED using the SysTick interrupt
 * on the STM32F407VG board. The datasheet used for this is the Cortex M4 Generic user 
 * guide, RM0090 and STM32F407xx datasheet.
 *
 * ---- Configuration steps ----
 * Disable all interrupts
 * First configure the RCC register for GPIO
 * Configure mode as output for LED
 * Configure the systick load, value and control registers
 * Enable all interrupts
 * Use the SysTick_Handler function for interrupt task
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 02/12/20
 *
 * NOTE: To work out the value to load into the SysTick timer do load value = delay / (1/fclk)
 * 		 Example to create a 200ms delay, load value = 0.2 / (1/16MHz)
 */

#include "stm32f407xx.h"

int main()
{
	__disable_irq();
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODE12_0;
	
	SysTick->LOAD = 3200000 - 1; 	// Start value to load into the SysTick Reload Value Register <CHECK NOTES>
	SysTick->VAL = 0;		// Clear the Current Value Register
	
	// Enable the clksource, tickint and enable SysTick
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	
	__enable_irq();
	
	while (1)
	{
		
	}
}

void SysTick_Handler(void)
{
	GPIOD->ODR ^= GPIO_ODR_OD12;
	
}
