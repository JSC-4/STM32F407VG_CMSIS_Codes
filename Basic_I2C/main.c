/* 
 * This program is a basic use of I2C to read the seconds of the DS3231 
 * module on the STM32F407VG board. The datasheet used for this is the 
 * Cortex M4 Generic user guide, RM0090 and STM32F407xx datasheet.
 *
 * ---- Configuration steps ----
 * Initialise I2C
 ** Enable RCC for GPIO where I2C is connected
 ** Set the GPIO mode register for I2C pins as AF
 ** Set the AF register to I2C
 ** Set the Output type as open-drain
 ** Enable I2C clock register
 ** Reset the I2C registers with SWRST bit
 ** Set the peripheral clock frequency in the CR2 register
 ** Set I2C clock to 100KHz (check note(1))
 ** Set the TRISE register bits (check note(2))
 ** Enable the I2C in the CR1 register
 *
 * Author: Jerome Samuels-Clarke
 * Website: www.jscblog.com
 * Last Modified 11/12/20
 *
 * NOTE(1): To work out the value for the I2C clock in the CR2 register
 *					do: Thigh = Tlow = CCR*Tpclk1. To get Thigh/Tlow check datasheet of module
 *					but seems like it's best to keep them the same value. In this example
 *					Thigh/Tlow = 5us, Tplck1 = 1/Fplck1 = 1/16MHz. CCR = 5us/(1/16MHz) = 80
 * 		 			Example to create a 1hz counter prescaler = 1600 - 1, ARR = 10000 -1 (16MHz/(1600*10000))
 * NOTE(2): To work out TRISE do 1000ns/(1/16MHz)+1, so in this example it is 16+1=7
 * NOTE(3): Checking if the bit is set, is also the same as reading the register i.e resetting the register. 
 */

#include "stm32f4xx.h"                  // Device header

#define SLAVE_ADDR 		0x68

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_addr_r(unsigned char addr);
void i2c_addr_w(unsigned char addr);
void i2c_writebyte(unsigned char c);
unsigned char i2c_readbyte(int ack);

volatile int seconds;

int i2c_readbyte2(char saddr, char maddr);

// Convert the bcd registers to decimal
int bcd2dec(char b) { return ((b/16)*10 + (b%16)); }

int main()
{
	
	i2c_init();
	
	while (1)
	{
		i2c_start();
		i2c_addr_w(SLAVE_ADDR);
		i2c_writebyte(0);
		i2c_start();
		i2c_addr_r(SLAVE_ADDR);
		seconds = bcd2dec(i2c_readbyte(0));
		i2c_stop();
	}
	
}

void i2c_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;		// Turn on GPIOB
	GPIOB->MODER |= GPIO_MODER_MODE7_1 | GPIO_MODER_MODE6_1;			// Set PB6 (SCL) PB7 (SDA) as AF
	GPIOB->AFR[0] |= GPIO_AFRL_AFRL7_2 | GPIO_AFRL_AFRL6_2;		// Set alternate functions for I2C1
	GPIOB->OTYPER |= GPIO_OTYPER_OT7 | GPIO_OTYPER_OT6;		// Set output as open-drain
	
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;		// Enable I2C1

	// Software reset I2C
	I2C1->CR1 |= I2C_CR1_SWRST;		
	I2C1->CR1 &= ~I2C_CR1_SWRST;		

	I2C1->CR2 |= I2C_CR2_FREQ_4;		// Set peripheral clock frequency to 16MHz
	I2C1->CCR |= 80;		// Set I2C clock to 100KHz (check note(1))
	I2C1->TRISE = 17;		// Set TRISE + 1 (check note(2))
	I2C1->CR1 |= I2C_CR1_PE;		// Enable I2C1
	
}

void i2c_writebyte(unsigned char c)
{
	while (!(I2C1->SR1 & I2C_SR1_TXE)){}		// Wait for TX register to be empty
	I2C1->DR = c;		// Write data to the data register
	while (!(I2C1->SR1 & I2C_SR1_TXE));		// Wait for TX register to be empty
}

unsigned char i2c_readbyte(int ack)
{
	/* Check if acknowledge bit is set, then clear it (might not be needed
		 and just to clear the acknowledge bit) */
	if (ack)
		I2C1->CR1 |= I2C_CR1_ACK;
	else
		I2C1->CR1 &= ~I2C_CR1_ACK;
	
	while (!(I2C1->SR1 & I2C_SR1_RXNE)){};	// Wait for RX register to be empty
	
	return (I2C1->DR);		// Return receive data
}

void i2c_addr_r(unsigned char addr)
{
	volatile char res;
	
	I2C1->DR = addr << 1 | 1; 	// Send slave address in write mode
	while (!(I2C1->SR1 & I2C_SR1_ADDR)){}; 	// Wait for addr bit flag (check note(3))

	I2C1->CR1 &= ~0x400;		// Clear the acknowledge bit
	res = I2C1->SR2;		// Clear SR2 register by reading it
}

void i2c_addr_w(unsigned char addr)
{
	volatile char res;
	
	I2C1->DR = addr << 1;		// Send slave address in write mode
	while (!(I2C1->SR1 & I2C_SR1_ADDR));	// Wait for addr bit flag (check note(3))
	
	res = I2C1->SR2;		// Clear SR2 register by reading it
}

void i2c_stop(void)
{
	I2C1->CR1 |= I2C_CR1_STOP;		// Send stop bit
	while (I2C1->SR2 & I2C_SR2_BUSY){}		// Wait for SB flag
}

void i2c_start(void)
{
	I2C1->CR1 |= I2C_CR1_START;			// Send start bit
	while (!(I2C1->SR1 & I2C_SR1_SB)){};		// Wait for start bit flag (check note(3))
}
