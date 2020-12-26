/*
 * This program is a basic sketch of using ADC for the STM32F407VG board.
 * The datasheet used for this is the RM0090 and STM32F407xx datasheet
 *
 * ---- Configuration steps ----
 * First configure the RCC register for the GPIO
 * Set GPIO pin mode to analog
 * Disable PUPDR on analog pin
 * Turn on ADC clock
 * Enable EOC interrupt and enable ADC NVIC
 * Set sample rate
 * Set sequencer to 0 (check notes)
 * Turn on ADC and continuous mode
 * Start ADC from SWSTART bit
 * In the interrupt 
 * Check to see if EOC is set
 * Reading from the data register clears interrupt flag
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 07/12/20
 *
 * NOTE(1): First sequence set to channel zero
 */
 
#include "stm32f407xx.h"

int analogValue;

int main()
{
	// Turn on GPIOA clock, set PA0 to analog mode and remove PUPDR
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODE0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0;
    
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// Turn on ADC1 clock
	ADC1->CR1 |= ADC_CR1_EOCIE;		// Turn on end of conversion interrupt
	NVIC_EnableIRQ(ADC_IRQn);		// Enable ADC interrupt
	ADC1->SMPR2 |= ADC_SMPR2_SMP0;		// Set to maximum sample rate
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;			// Set sequencer to 0 (check notes)
	ADC1->CR2 |= ADC_CR2_ADON | ADC_CR2_CONT;		// Turn on ADC and set to continuous mode
	ADC1->CR2 |= ADC_CR2_SWSTART;		// Start ADC
	
	while (1)
	{

	}
	
}

void ADC_IRQHandler(void)
{
	if (ADC1->SR & ADC_SR_EOC)
		analogValue = ADC1->DR;
	
}

