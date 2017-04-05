/*
 * main.c
 *
 */


#include "tm1637.h"


int main(void)
	{
	TM1637_SERIAL_INIT;

	led_print(0, "1620");
	led_dots(1);

	while(1)
		{

		}
	}
