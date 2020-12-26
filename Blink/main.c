/* 
 * This program is a basic blink led sketch for the STM32F407VG board.
 * The datasheet used for this is the RM0090 and STM32F407xx datasheet.
 * 
 * ---- Configuration steps ----
 * First configure the RCC register for the LED (PD13)
 * Then set the mode to general purpose output mode
 * Set/reset ODR register
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 20/11/20
 */
 
#include "stm32f407xx.h"

int main(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	
	GPIOD->MODER |= GPIO_MODER_MODE13_0;
	GPIOD->MODER &= ~GPIO_MODER_MODE13_1;

	while (1)
	{
		GPIOD->ODR ^= (1 << 13);
		for (int i=0; i<500000; i++);
	}
}
