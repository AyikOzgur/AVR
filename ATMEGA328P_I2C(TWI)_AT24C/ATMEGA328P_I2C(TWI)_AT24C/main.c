/*
 * ATMEGA328P_I2C(TWI)_AT24C.c
 *
 * Created: 2.03.2023 16:44:34
 * Author : ozgur
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define get_bit(reg, bitnum) ((reg & (1 << bitnum)) >> bitnum)
void TWI_Init(void);
void EEPROM_Write(char data, uint16_t address);
void EEPROM_READP(uint16_t address, char *data);

char write_data = 'A';
char read_data;

int main(void)
{
	/* Replace with your application code */
	TWI_Init();
	_delay_ms(100);
	EEPROM_Write(write_data, 0x0001);
	_delay_ms(100);
	EEPROM_READP(0x0001, &read_data);

	while (1)
	{
	}
}

void TWI_Init(void)
{

	TWBR = 0b01001000; // For 100 KHz SCL Frequancy
	TWCR |= (1 << TWEN);
}

void TWI_Start(void)
{

	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	while (get_bit(TWCR, TWINT) == 0)

	{
	}
}

void TWI_Stop(void)
{

	TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
}

void TWI_Write(char data)
{

	TWDR = data;

	TWCR = (1 << TWINT) | (1 << TWEN);

	while (get_bit(TWCR, TWINT) == 0)

	{
	}
}

char TWI_Read_ack(void)
{

	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); /* Enable TWI, generation of ack */
	while (!(TWCR & (1 << TWINT)))
		; /* Wait until TWI finish its current job */
	return TWDR;
}

char TWI_Read_Nack(void)
{
	TWCR = (1 << TWEN) | (1 << TWINT); /* Enable TWI and clear interrupt flag */
	while (!(TWCR & (1 << TWINT)))
		;		 /* Wait until TWI finish its current job */
	return TWDR; /* Return received data */
}

void EEPROM_Write(char data, uint16_t address)

{
	uint8_t dataaddress[2];
	dataaddress[0] = address >> 8;
	dataaddress[1] = address & 0x00FF; // decompose address into two 8bit datas
	TWI_Start();
	TWI_Write(0xA0); // Address of EEPROM with write bit
	TWI_Write(dataaddress[0]);
	TWI_Write(dataaddress[1]);
	TWI_Write(data); // send the data
	TWI_Stop();
}

void EEPROM_READP(uint16_t address, char *data)
{
	uint8_t dataaddress[2];
	dataaddress[0] = address >> 8;
	dataaddress[1] = address & 0x00FF; // decompose address into two 8bit datas

	TWI_Start();			   // Start bit
	TWI_Write(0xA0);		   // Address of EEPROM with write bit
	TWI_Write(dataaddress[0]); // MSB of data address
	TWI_Write(dataaddress[1]); // LSB of data address
	TWI_Start();			   // Repeated start
	TWI_Write(0xA1);		   // Address of EEPROM with read bit
	*data = TWI_Read_ack();
	TWI_Stop();
}