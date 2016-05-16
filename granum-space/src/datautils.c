

#include "spi.h"
#include "sd.h"
#include "uart_stdio.h"
#include <util/delay.h>

uint32_t block = 0;
uint16_t bib = 0;

void du_init() {
	while(1) {
		if(sd_init()==0x00) {
			break;
		}
		_delay_ms(100);
	}
}

void du_write(const void* data, int length) {
	const uint8_t* ptr = (const uint8_t*) data;
	for(int i = 0; i < length; i++) {
		if(bib == 0) {
			uint8_t CMD24[] = {
				0x58,
				(uint8_t)((block >> 24) & 0xFF),
				(uint8_t)((block >> 16) & 0xFF),
				(uint8_t)((block >> 8) & 0xFF),
				(uint8_t)(block & 0xFF),
				0x00 //CRC8
			};
			uint8_t Answ[6];
			spi_exchange(CMD24, sizeof(CMD24),Answ);
			for(int i = 0; i<6;i++) DEBUG("CMD24 transfer %d: %d\n", i, Answ[i]);
			DEBUG("Start of new packet\n");
			while(1) {
				uint8_t answer = spi_sendbyte(0xFF);
				DEBUG("CMD24 answer %d\n", answer);
				if(answer==0x00) break;
			}
			spi_sendbyte(0xFF);
			spi_sendbyte(~(0x01));
			spi_sendbyte((uint8_t)(*(ptr+i) & 0xFF));
			bib++;
		}
		else {
			if(bib < 511) {
				spi_sendbyte((uint8_t)(*(ptr+i) & 0xFF));
				bib++;
			}
		}
		if(bib == 511) {
			spi_sendbyte((uint8_t)(*(ptr+i) & 0xFF));
			spi_sendbyte(0xFF);
			spi_sendbyte(0xFF);
			DEBUG("It data answer was %d\n",spi_sendbyte(0xFF));
			while(1) {
				uint8_t answer = spi_sendbyte(0xFF);
				DEBUG("%d\n", answer);
				if(answer) break;
			}
			bib = 0;
			block += 512;
		}
	}
}
