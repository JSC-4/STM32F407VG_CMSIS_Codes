#include "stm32f4xx.h"                  // Device header
#include "DS3231.h"

#define SLAVE_ADDR 		0x68


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

static void i2c_writebyte(uint8_t byte)
{
	while (!(I2C1->SR1 & I2C_SR1_TXE)){}		// Wait for TX register to be empty
	I2C1->DR = byte;		// Write data to the data register
	while (!(I2C1->SR1 & I2C_SR1_TXE));		// Wait for TX register to be empty
}

static uint8_t i2c_readbyte(Ack_send ack)
{
	// To read multiple bytes set ack to 1
	if (ack)
		I2C1->CR1 |= I2C_CR1_ACK;
	else
		I2C1->CR1 &= ~I2C_CR1_ACK;
	
	while (!(I2C1->SR1 & I2C_SR1_RXNE)){};	// Wait for RX register to be empty
	
	return (I2C1->DR);		// Return receive data
}

void i2c_addr_r(uint8_t addr)
{
	volatile char res;
	
	I2C1->DR = addr << 1 | 1; 	// Send slave address in read mode
	while (!(I2C1->SR1 & I2C_SR1_ADDR)){}; 	// Wait for addr bit flag (check note(3))

	I2C1->CR1 &= ~0x400;		// Clear the acknowledge bit
	res = I2C1->SR2;		// Clear SR2 register by reading it
}

void i2c_addr_w(uint8_t addr)
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

// Convert the bcd registers to decimal
static int bcd2dec(uint8_t b) { return ((b/16)*10 + (b%16)); }
static int decimal2bcd (uint8_t d) { return (((d/10) << 4) | (d % 10)); }

uint8_t ds3231_seconds(void)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(0);
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	i2c_stop();
	return bcd2dec(i2c_readbyte(MULTI_BYTE_OFF));
}

uint8_t ds3231_minutes(void)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(1);
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	i2c_stop();
	return bcd2dec(i2c_readbyte(MULTI_BYTE_OFF));
}

uint8_t ds3231_hours(void)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(2);
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	i2c_stop();
	return bcd2dec(i2c_readbyte(MULTI_BYTE_OFF));
}

void ds3231_get_time(ds3231_data_struct *ds3231_struct)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(0);
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	ds3231_struct->seconds = bcd2dec(i2c_readbyte(MULTI_BYTE_ON));
	ds3231_struct->minutes = bcd2dec(i2c_readbyte(MULTI_BYTE_ON));
	ds3231_struct->hours = bcd2dec(i2c_readbyte(MULTI_BYTE_OFF));
	i2c_stop();	
}

void ds3231_get_date(ds3231_data_struct *ds3231_struct)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(4);
	i2c_start();
	i2c_addr_r(SLAVE_ADDR);
	ds3231_struct->date = bcd2dec(i2c_readbyte(MULTI_BYTE_ON));
	ds3231_struct->month = bcd2dec(i2c_readbyte(MULTI_BYTE_ON));
	ds3231_struct->year = bcd2dec(i2c_readbyte(MULTI_BYTE_OFF));
	i2c_stop();		
}

void ds3231_set_time(uint8_t hh, uint8_t mm, uint8_t ss)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(0);
	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	// Wait for TX register to be empty
		
	I2C1->DR = decimal2bcd(ss);
	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	// Wait for TX register to be empty
		
	I2C1->DR = decimal2bcd(mm);
	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	// Wait for TX register to be empty
	
	I2C1->DR = decimal2bcd(hh);
	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	// Wait for TX register to be empty
	
	i2c_stop();
}

void ds3231_set_date(uint8_t dd, uint8_t mm, uint8_t yy)
{
	i2c_start();
	i2c_addr_w(SLAVE_ADDR);
	i2c_writebyte(4);
	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	// Wait for TX register to be empty
		
	I2C1->DR = decimal2bcd(dd);
	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	// Wait for TX register to be empty
		
	I2C1->DR = decimal2bcd(mm);
	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	// Wait for TX register to be empty
	
	I2C1->DR = decimal2bcd(yy);
	while (!(I2C1->SR1 & I2C_SR1_TXE)){};	// Wait for TX register to be empty
	
	i2c_stop();	
}
