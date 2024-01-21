/*
 * ATMEGA328P_ADC_USART.c
 *
 * Created: 2.03.2023 15:50:21
 * Author : ozgur
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void serial_init();
void serial_sends(char *s);
void serial_send(unsigned char data);
void adc_init(void);
int adc_read(void);

int main(void)
{

	char data[31];
	int adc_value = 0;
	DDRC &= ~(1 << 5); // PC5 is input, It is connected ADC chanel 5

	serial_init();
	adc_init();

	while (1)
	{
		adc_value = adc_read();
		sprintf(data, "ADC value is : %d \n \r", adc_value);
		serial_sends(data);
		_delay_ms(1);
	}
}
void adc_init(void)
{
	ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)); // 125Khz ADC reference clock
	ADMUX |= (1 << REFS0);									// Reference is AVCC so 5V
	ADCSRA |= (1 << ADEN);									// enable ADC
	ADCSRA |= (1 << ADSC);									// Do first conversion to make ADC ready
}

int adc_read(void)
{
	ADMUX &= 0XF0;
	ADMUX |= (1 << 2) | (1 << 0); // Chanel 5
	ADMUX &= ~(1 << 3);
	ADMUX &= ~(1 << 1);
	ADCSRA |= (1 << ADSC); // start conversion
	while (!(ADCSRA & (1 << ADIF)))
		; // wait conversion to be finished

	return ADCW; // 16bit data register which contanins ADCH and ADCL.
}

void serial_init()
{
	UBRR0 = UBRR_VALUE;						 // baud rate
	UCSR0B |= (1 << TXEN0);					 // TX avtivate
	UCSR0B |= (1 << RXEN0);					 // RX activate
	UCSR0B |= (1 << RXCIE0);				 // RX complete interrupt
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ01); // no parity, 1 stop bit, 8-bit data
}

void serial_sends(char *s)
{
	// send whole array till reach null
	while (s > 0)
		serial_send(s++);
}

void serial_send(unsigned char data)
{
	UDR0 = data; // send the data
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}; // wait data to be sent
}
