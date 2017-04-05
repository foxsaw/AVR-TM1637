//
// TM1637  4-digit LED display  "88:88"
//
// avr-gcc  ATmega328P
//
// toxcat :3  Copyleft  03-Apr-2017
//



#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>



///////////////////////////////////////////////////////////////////////////////

#define TM1637_DIO_PIN    7
#define TM1637_DIO_PORT   PORTD

#define TM1637_CLK_PIN    6
#define TM1637_CLK_PORT   PORTD

///////////////////////////////////////////////////////////////////////////////


#define SET_BIT(reg, bit) (reg |= (1<<bit))
#define CLR_BIT(reg, bit) (reg &= (~(1<<bit)))

#define BIT_IS_SET(reg, bit) ((reg>>bit)&1)
#define BIT_IS_CLR(reg, bit) (!((reg>>bit)&1))


#define DDR_REG(x)  (*(&x-1))
#define PIN_REG(x)  (*(&x-2))


#define DIO_INP  CLR_BIT(DDR_REG(TM1637_DIO_PORT), TM1637_DIO_PIN)
#define DIO_OUT  SET_BIT(DDR_REG(TM1637_DIO_PORT), TM1637_DIO_PIN)

#define DIO_SET  SET_BIT(TM1637_DIO_PORT, TM1637_DIO_PIN)
#define DIO_CLR  CLR_BIT(TM1637_DIO_PORT, TM1637_DIO_PIN)

#define DIO_IS_SET  BIT_IS_SET(PIN_REG(TM1637_DIO_PORT), TM1637_DIO_PIN)
#define DIO_IS_CLR  BIT_IS_CLR(PIN_REG(TM1637_DIO_PORT), TM1637_DIO_PIN)


#define CLK_INP  CLR_BIT(DDR_REG(TM1637_CLK_PORT), TM1637_CLK_PIN)
#define CLK_OUT  SET_BIT(DDR_REG(TM1637_CLK_PORT), TM1637_CLK_PIN)

#define CLK_SET  SET_BIT(TM1637_CLK_PORT, TM1637_CLK_PIN)
#define CLK_CLR  CLR_BIT(TM1637_CLK_PORT, TM1637_CLK_PIN)


#define TM1637_SERIAL_INIT  CLK_CLR; CLK_INP; DIO_CLR; DIO_INP


#define CLK_H  CLK_INP
#define CLK_L  CLK_OUT

#define DIO_H  DIO_INP
#define DIO_L  DIO_OUT


#define HIGH  1
#define LOW   0


#define DATA_COMMAND     0b01000000
#define AUTO_ADDRESS     0b00000000
#define FIXED_ADDRESS    0b00000100
#define WRITE_DATA       0b00000000
#define READ_DATA        0b00000010

#define ADDRESS_COMMAND  0b11000000
#define DEFAULT_ADDRESS  0b00000000

#define DISPLAY_COMMAND  0b10000000
#define DISPLAY_ON       0b00001000
#define DISPLAY_OFF      0b00000000
#define DISPLAY_BRIGHT   0b00000010


#define SERIAL_DELAY _delay_us(10)


uint8_t sbuff[4] = { 0x00, 0x00, 0x00, 0x00 };

const uint8_t scode[] PROGMEM = {
  0b00111111,  // 0   // 0
  0b00000110,  // 1   // 1
  0b01011011,  // 2   // 2
  0b01001111,  // 3   // 3
  0b01100110,  // 4   // 4
  0b01101101,  // 5   // 5
  0b01111101,  // 6   // 6
  0b00000111,  // 7   // 7
  0b01111111,  // 8   // 8
  0b01101111,  // 9   // 9
  0b00000000,  // 10  // space
};



//-----------------------------------------------------------------------------
void serial_begin(void)
	{
	SERIAL_DELAY;
	DIO_L;
	SERIAL_DELAY;
	}


//-----------------------------------------------------------------------------
void serial_end(void)
	{
	CLK_L;
	SERIAL_DELAY;
	DIO_L;
	SERIAL_DELAY;

	CLK_H;
	SERIAL_DELAY;
	DIO_H;
	SERIAL_DELAY;
	}


//-----------------------------------------------------------------------------
void serial_cycle(uint8_t data)
	{
	CLK_L;
	SERIAL_DELAY;

	if(data) { DIO_H; } else { DIO_L; }
	SERIAL_DELAY;

	CLK_H;
	SERIAL_DELAY;
	SERIAL_DELAY;
	}


//-----------------------------------------------------------------------------
uint8_t serial_write(uint8_t data)
	{
	uint8_t ack=HIGH;

	for(uint8_t mask=1; mask; mask<<=1)  serial_cycle(data & mask);

	serial_cycle(HIGH);

	if(DIO_IS_CLR) ack=LOW;

	return ack;
	}


//-----------------------------------------------------------------------------
void led_update(void)
	{
	serial_begin();
	serial_write(DATA_COMMAND | AUTO_ADDRESS | WRITE_DATA);
	serial_end();

	serial_begin();
	serial_write(ADDRESS_COMMAND | DEFAULT_ADDRESS);

	for(uint8_t k=0; k<4; k++) serial_write(sbuff[k]);

	serial_end();

	serial_begin();
	serial_write(DISPLAY_COMMAND | DISPLAY_ON | DISPLAY_BRIGHT);
	serial_end();
	}


//-----------------------------------------------------------------------------
void led_char(uint8_t pos, uint8_t sign)
	{
	uint8_t tmp=0;

	if(sign>=48 && sign<=57) tmp=(pgm_read_byte(&scode[sign-48]));  //digit
	else tmp=(pgm_read_byte(&scode[10]));  //space

	if(pos<4) sbuff[pos]=(tmp|(sbuff[pos]&0b10000000));
	}


//-----------------------------------------------------------------------------
void led_print(uint8_t pos, char *str)
	{
	for(;((*str) && (pos<4));) led_char(pos++,*str++);

	led_update();
	}


//-----------------------------------------------------------------------------
void led_dots(uint8_t on)
	{
	if(on) SET_BIT(sbuff[1],7);
	else CLR_BIT(sbuff[1],7);

	led_update();
	}

