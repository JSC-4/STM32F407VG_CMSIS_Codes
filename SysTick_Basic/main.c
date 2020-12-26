/* 
 * This program is a basic use of SysTick to blink an LED
 * on the STM32F407VG board. The datasheet used for this is the 
 * Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 *
 * ---- Configuration steps ----
 * First configure the RCC register for GPIO
 * Configure mode as output for LED
 * Configure the systick load, value and control registers
 * Check to see if count flag has been set in the systick control register
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 22/11/20
 *
 * NOTE: To work out the value to load into the SysTick timer do load value = delay / (1/fclk)
 * 		 Example to create a 200ms delay, load value = 0.2 / (1/16MHz)
 */
 
 #include "stm32f407xx.h"
 
	int main()
	{
        // Configure LED as output, by turning on clock and setting mode register
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
		GPIOD->MODER |= GPIO_MODER_MODE15_0;
	 
		SysTick->LOAD = 3200000 - 1;			// Start value to load into the SysTick Reload Value Register <CHECK NOTES>
		SysTick->VAL = 0;									// Clear the Current Value Register
		SysTick->CTRL |= (0x5 << 0);			// Enable the SysTick Control and Status Register and set clksource to processor clock
	 
		while (1)
		{
			if (SysTick->CTRL & (1 << 16)) // Check to see if COUNTFLAG was set
				GPIOD->ODR ^= (1<<15);
		}
	 
	}
 