/* 
 * This program is a basic use of SPI with on the on-board LIS3DSH
 * accelerometer sensor, using the STM32F407VG board. The datasheet 
 * used for this is the Cortex M4 Generic user guide, RM0090, STM32F407xx 
 * datasheet, discovery board UM1472, LIS3DSH datasheet and application notes.
 *
 * ---- Configuration steps ----
 * Configure the RCC register for GPIOA and GPIOE
 * Set PE3 as output (CS)
 * Set GPIOA SPI pins using the datasheets for AF
 * Enable SPI clock
 * Set SPI mode in the CR1 register
 * Enable master mode, SSM and SSI in the CR1 register
 * Clear CR2 register
 * Enable SPI in the CR1 register
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 29/01/21
 *
 */
 
#include "stm32f4xx.h"                  // Device header

void SPI_Init(void);
static __inline	unsigned char spi_rw_data(unsigned char data);

int main(void)
{
	// Variables for storing ADC readings
	char readings[6] = {0};
	volatile short acc_x, acc_y, acc_z;
	
	SPI_Init();
	
	//Turn on FIFO Mode
	GPIOE->ODR &= ~GPIO_ODR_OD3;
	spi_rw_data(0x25);		
	spi_rw_data(0x50);
	GPIOE->ODR |= GPIO_ODR_OD3;
	
	// Activate stream mode
	GPIOE->ODR &= ~GPIO_ODR_OD3;
	spi_rw_data(0x2E);		
	spi_rw_data(0x40);
	GPIOE->ODR |= GPIO_ODR_OD3;	
	
	// Start up sequence taken from LIS3DSH application notes
	// Enable the  X, Y, Z axis and set ODR to 100HZ
	GPIOE->ODR &= ~GPIO_ODR_OD3;
	spi_rw_data(0x20);		
	spi_rw_data(0x67);
	GPIOE->ODR |= GPIO_ODR_OD3;

/* Taken from the application notes, to turn on interrupt if needed */
//	GPIOE->ODR &= ~GPIO_ODR_OD3;
//	spi_rw_data(0x23);		
//	spi_rw_data(0xC8);
//	GPIOE->ODR |= GPIO_ODR_OD3;

	while (1)
	{
		// Start the acceleration reading
		GPIOE->ODR &= ~GPIO_ODR_OD3;
		spi_rw_data(0xA8);		// start at OUT_X_L
		readings[0] = spi_rw_data(0);
		readings[1] = spi_rw_data(0);
		readings[2] = spi_rw_data(0);
		readings[3] = spi_rw_data(0);
		readings[4] = spi_rw_data(0);
		readings[5] = spi_rw_data(0); // End at OUT_Z_H
		GPIOE->ODR |= GPIO_ODR_OD3;
		
		// Put readings to 16 bit format
		acc_x = (readings[1] << 8) | (readings[0]);
		acc_y = (readings[3] << 8) | (readings[2]);
		acc_z = (readings[5] << 8) | (readings[4]);

	}
	
}

void SPI_Init(void)
{
	// Turn on clock for GPIOE (SPI1/CS)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	
	// Set PE3 (CS) as an output
	GPIOE->MODER &= ~(GPIO_MODER_MODE3);
	GPIOE->MODER |= GPIO_MODER_MODE3_0;
	
	// Turn on clock for GPIOA (SPI1)
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	// Configure PA5 (SCLK), PA6 (SPI_MISO) and PA7 (MOSI) as alternate functions
	GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);	
	GPIOA->MODER |= GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1;
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL5_0 | GPIO_AFRL_AFRL5_2 | GPIO_AFRL_AFRL6_0 | GPIO_AFRL_AFRL6_2 | GPIO_AFRL_AFRL7_0 | GPIO_AFRL_AFRL7_2;
	
	// Turn on SPI1 enable
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	// Set to SPI mode 3 (CPOL = 1, CPHA = 1)
	SPI1->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
	
	// Set SPI to master mode
	SPI1->CR1 |= SPI_CR1_MSTR;

	// Enable SSM and SSI
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
	
	// Clear the CR2 register
	SPI1->CR2 = 0;
	
	// Enable SPI1
	SPI1->CR1 |= SPI_CR1_SPE;
}

static __inline	unsigned char spi_rw_data(unsigned char data)
{
	SPI1->DR = data;

	while (!(SPI1->SR & SPI_SR_RXNE));	/* Wait for send to finish */
	return (SPI1->DR);
	}
