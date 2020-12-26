/* 
 * This program is basic use using a timer to trigger an ADC reading
 * on the STM32F407VG board. The datasheet used for this is the 
 * Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 * 
 * ---- Configuration steps ----
 * ADC Configure
 ** Configure the RCC register for GPIO
 ** Set ADC pin as analog (check NOTE(1))
 ** Enable ADC clock
 ** Set EOCIE bit and enable ADC NVIC interrupt in CR1
 ** Set external trigger as rising edge in CR2
 ** Set external select to Trigger 2 Trigger Event in CR2
 ** Set channel conversion length in SQR1
 ** Select channel for conversion in SQR3
 ** Set sampling time (check NOTE(2)) in SMPx register
 ** Enable ADON and SWSTART in CR2
 *
 * Timer Configure
 ** Enable timer clock
 ** Configure the timer prescale and autoload registers
 ** Clear and set MMS bits as update
 ** Reset counter value and enable timer
 *
 * NOTE(1): Pull-up/Pull-down bit for ADC pin seems to not have much effect when set or not set
 * NOTE(2): To work out sampling time ADC clock is Fplck2/2 (default prescaler) = 16MHz/2 = 8MHz
 *					Tconv_c = sampling + 12 cycles, sampling set to 28, meaning Tconv_c = 40
 *					Tconv_t = ADC clock * Tconv_c = (1/8MHz) * 40 = 5us
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 20/12/20
 *
 */

#include "stm32f4xx.h"  // Device header

uint32_t analogValue;	// Store analog value
uint32_t timerVal;		// Store timer value

void ADC_Config(void);
void Timer_Config(void);

int main(void)
{
	ADC_Config();
	Timer_Config();
	
	// Configure LED (PD13)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODE13_0;
	
	while (1)
	{
		timerVal = TIM2->CNT;
	}

}

void ADC_Config(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;		// Turn on GPIOA clock
	GPIOA->MODER |= GPIO_MODER_MODE0;			// Set PA0 to analog mode
	//GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;		// No PUPD (check NOTE(1))
	
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// Enable ADC1 clock

	ADC1->CR1 |= ADC_CR1_EOCIE;		// Turn on end of conversion interrupt
	NVIC_EnableIRQ(ADC_IRQn);		// Enable ADC interrupt

	ADC1->CR2 |= ADC_CR2_EXTEN_0;	// External trigger on rising edge
	ADC1->CR2 |= ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1;	// Set Timer 2 Trigger Event

	ADC1->SQR1 &= ~ADC_SQR1_L;		// Sequence channel length set to 1 conversion 
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;		// Select channel 0 for conversion
	ADC1->SMPR2 |= ADC_SMPR2_SMP0_1; // Sampling set for 5us (check NOTE(2))
	
	ADC1->CR2 |= ADC_CR2_ADON;		// Turn on ADC
	ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversion
	
}

void Timer_Config(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	// Enable Timer 2 clock
	
	// Configure timer for a 1Hz reading
	TIM2->PSC = 16000 - 1;
	TIM2->ARR = 1000 - 1;
	
	// Clear MMS bits and set as update
	TIM2->CR2 &= ~TIM_CR2_MMS;
	TIM2->CR2 |= TIM_CR2_MMS_1;
	
	TIM2->CNT = 0;		// Reset counter value
	TIM2->CR1 |= TIM_CR1_CEN;		// Enable Timer 2;
}

void ADC_IRQHandler(void)
{
	if (ADC1->SR & ADC_SR_EOC)	// Check if the EOC flag has been set
	{
		analogValue = ADC1->DR;		// Reading the data register clears the flag EOC flag
		GPIOD->ODR ^= GPIO_ODR_ODR_13;		// Toggle LED
	}
}
