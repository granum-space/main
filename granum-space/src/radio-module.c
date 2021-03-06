


#include "radio-module.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "uart-debug.h"
#include "config.h"
#include "adc.h"

bool radio_needinit = true;

uint8_t rxbuf[10];
int rxbufi;
bool OnLaunchpad = true;
uint16_t StartPres;

ISR (USART0_RX_vect) {
	rxbuf[rxbufi] = UDR0;
	rxbufi++;
	if(rxbufi == 4) {
		if((rxbuf[0]==RXMES0) && (rxbuf[1]==RXMES1) && (rxbuf[2]==RXMES2) && (rxbuf[3]==RXMES3)) {
			OnLaunchpad = true;
			StartPres = adc_read(ADC_CHANNEL_PRESSURE);
		}
		rxbufi = 0;
	}
	if(rxbufi == 9) rxbufi = 0;
}

void radio_write(const uint8_t *value, size_t size){

	for(size_t i = 0; i < size; i++){
		while(CTSPIN & (1<<CTSLEG)) {}//GR_DEBUG("CTS wait\n");
		while ( !(UCSR0A & (1 << UDRE0)) ) {}//GR_DEBUG("FREE WAIT\n");
		//GR_DEBUG("Wait complete\n");
		UDR0 = *(value+i);
	}
}

void radio_init(){
	if(radio_needinit){
		sei();
		PDDDR |=(1<<PDLEG);
		PDPORT |= (1<<PDLEG);

		RTSDDR |= (1<<RTSLEG);
		RTSPORT &= ~(1<<RTSLEG);

		CFGDDR = CFGDDR | (1 << CFGLEG);
		CTSDDR = CTSDDR & ~(1 << CTSLEG);

		// инициализация уарт
		UCSR0B = (1 << TXEN0) | (1<<RXEN0) |(1<<RXCIE0);// включаем только TX //(нет)

		UCSR0C = (1 << UCSZ00) | (1 << UCSZ01) // Размер символа - 8 бит
			| (0 << UPM00) | (0 << UPM01)      // Бит четности отключен
			| (0 << USBS0) // 1 стоп бит
		;

		UBRR0L = (int)(F_CPU/(16.0*RXQ2_BAUD_RATE)-1) % 0xFF;
		UBRR0H = (int)(F_CPU/(16.0*RXQ2_BAUD_RATE)-1) / 0xFF;

		CFGPORT = CFGPORT | (1 << CFGLEG);
		uint8_t test = 100;
		radio_write(&test , 1);
		_delay_ms(100);
		CFGPORT &= ~(1 << CFGLEG);

		uint8_t config[12] = {
			RF_TARGET_ADDR,
			RF_SELF_ADDR,
			RF_CH,
			RF_POWER,
			RF_TX_PACKET,
			RF_RX_PACKET,
		};

		radio_write(config, 1);
		_delay_ms(1000);
		radio_write(config, 12);
		_delay_ms(1000);
		radio_write(config, 1);
		CFGPORT = CFGPORT | (1 << CFGLEG);
		radio_needinit = false;
	}
}
