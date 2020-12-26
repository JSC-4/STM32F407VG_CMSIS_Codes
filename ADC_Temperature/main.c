/* DOES NOT WORK READ DATASHEET FOR HOW TO CONFIGURE TEMPERATURE SENSOR
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 08/12/20
 *
 */
 
#include "stm32f407xx.h"

int data;
double voltage;
double celsius;

int main ()
{

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// Enable ADC1
	ADC->CCR |= ADC_CCR_TSVREFE;
	ADC->CCR &= ~ADC_CCR_VBATE;
	
	ADC1->SMPR1 |= ADC_SMPR1_SMP16_2;
	ADC1->SQR3 = ADC_SQR3_SQ1_3 | ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_0; // Channel 16
	ADC1->CR2 |= ADC_CR2_EXTEN_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0;
	ADC1->CR2 |= ADC_CR2_ADON | ADC_CR2_CONT;

	while (1)
	{
		ADC1->CR2 |= ADC_CR2_SWSTART;

		while (!(ADC1->SR & ADC_SR_EOC)){}
		data = ADC1->DR;
		voltage = (double)data/4095*3.3;		// Convert ADC reading to voltage
		celsius = ((voltage - 0.76) / 0.0025) + 25;
	}
	
}
