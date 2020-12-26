/* 
 * This program is basic use using a timer to trigger an ADC reading
 * on the STM32F407VG board. The datasheet used for this is the 
 * Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 * 
 * ---- Configuration steps ----
 * ADC Configure
 ** Configure the RCC register for GPIO
 ** Set ADC pin as analog 
 ** Enable ADC clock
 ** Set sampling cycles in the SMPRx register
 ** Set channel conversion length in SQR1
 ** Set channel(s) for conversion in SQR3
 ** Enable scan mode in CR1
 ** Enable continuous mode in CR2
 ** Enable DMA and DDS bit in CR2
 ** Enable ADON in CR2
 ** Enable SWSTART in CR2
 *
 * DMA Configure
 ** Enable DMA clock
 ** Assign peripheral address casted to uint32_t e.g. (uint32_t)(&(ADC1->DR)) to PAR
 ** Pass array to memory 0 register e.g. (uint32_t)samples to M0AR
 ** Set number of transfers in NTDR register
 ** Set memory and peripheral size to 32 bits, set memory increment and circular mode in CR
 **	Select channel in CR register
 ** Enable DMA stream in CR
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 20/12/20
 *
 */
 
#include "stm32f407xx.h"

void DMA_Config(void);
void ADC_Config(void);

uint32_t samples[2]; // Array to hold ADC readings from DMA

int main()
{
		
	ADC_Config();
	DMA_Config();
	
	while (1)
	{

	}
	
}

void ADC_Config(void)
{
	// Turn on GPIOA clock and set PA0 and PA1 to analog mode
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODE1 | GPIO_MODER_MODE0;

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// Turn on ADC1 clock

	//ADC1->CR2 &= ~(ADC_CR2_ADON | ADC_CR2_SWSTART); // Turn ADC off (not needed)
	
	ADC1->SMPR2 |= ADC_SMPR2_SMP1 | ADC_SMPR2_SMP0;		// Set to sampling to 480
	ADC1->SQR1 |= ADC_SQR1_L_0;		// Set channel sequence to sample two channels
	
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;		// Set Sequencer 1 to sample channel 0 (check notes)
	ADC1->SQR3 |= ADC_SQR3_SQ2_0;  	// Set Sequencer 2 to sample channel 1 (check notes)
	
	ADC1->CR1 |= ADC_CR1_SCAN;		// Enable scan mode
	ADC1->CR2	|= ADC_CR2_CONT;	// Set continuous mode on

	// Enable DMA for ADC and set DDS bit
	ADC1->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS;
		
	ADC1->CR2 |= ADC_CR2_ADON;	// Turn on ADC and set to continuous mode
	ADC1->CR2 |= ADC_CR2_SWSTART;	// Start ADC
}

void DMA_Config(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;		// Enable DMA2
	
	DMA2_Stream0->PAR = (uint32_t)(&(ADC1->DR));	// Address of ADC
	DMA2_Stream0->M0AR = (uint32_t)samples; 	// Pass array where ADC values will be writted to
	DMA2_Stream0->NDTR = 2;		// Numbers of data items to transfer
	
	// Set memory and peripheral size to 32 bits, set memory increment and circular mode
	DMA2_Stream0->CR |= DMA_SxCR_MSIZE_1 | DMA_SxCR_PSIZE_1 | DMA_SxCR_MINC | DMA_SxCR_CIRC;
	DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL;	// Select channel 0
	DMA2_Stream0->CR |= DMA_SxCR_EN;	// Enable DMA stream
}
