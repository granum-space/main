#include <avr/io.h>
#include <stdbool.h>

#include "config.h"

bool spi_needinit = true;

void spi_init()
{
	if(spi_needinit){
	   /*настройка портов ввода-вывода
	   все выводы, кроме MISO выходы*/
	   SPI_DDRX |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS)|(0<<SPI_MISO);
	   SPI_PORTX |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS)|(1<<SPI_MISO);

	   /*разрешение spi,старший бит вперед,мастер, режим 0*/
	   SPCR = (1<<SPE)|(0<<DORD)|(1<<MSTR)|(0<<CPOL)|(0<<CPHA)|(1<<SPR1)|(0<<SPR0);
	   SPSR = (0<<SPI2X);
		spi_needinit = false;
	}
}

uint8_t spi_sendbyte(uint8_t arg) {
	SPDR = arg;
	while(!(SPSR & (1<<SPIF)));
	arg = SPDR;
	return arg;
}

void spi_exchange(const void* data, int length, uint8_t* retval) {
	const uint8_t* ptr = (const uint8_t*)data;
	for(int i = 0;i< length; i++){
		SPDR = *(ptr+i);
		while(!(SPSR & (1<<SPIF)));
		if(retval) retval[i] = SPDR;
	}
}
