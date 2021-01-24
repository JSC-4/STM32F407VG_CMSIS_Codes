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
	ds3231_data_struct ddw;
	
	i2c_init();
	
	ds3231_set_time(15, 28, 00);
	ds3231_set_date(12, 01, 21);
	
	while (1)
	{
		ds3231_get_time(&ddw);

		seconds = ddw.seconds;
		minutes = ddw.minutes;
		hours = ddw.hours;

		ds3231_get_date(&ddw);
		
		date 	= ddw.date;
		month = ddw.month;
		year 	= ddw.year;
	}	
	
}
