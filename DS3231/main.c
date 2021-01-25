#include "stm32f4xx.h"                  // Device header
#include "DS3231.h"

volatile int hours;
volatile int seconds;
volatile int minutes;

volatile int date;
volatile int month;
volatile int year;

int main(void)
{
	ds3231_data_struct ds3231_handle;
	
	i2c_init();
	
	ds3231_set_time(11, 28, 00);
	ds3231_set_date(25, 01, 21);
	
	while (1)
	{
		ds3231_get_time(&ds3231_handle);

		seconds = ds3231_handle.seconds;
		minutes = ds3231_handle.minutes;
		hours = ds3231_handle.hours;

		ds3231_get_date(&ds3231_handle);
		
		date 	= ds3231_handle.date;
		month = ds3231_handle.month;
		year 	= ds3231_handle.year;
	}	
	
}
