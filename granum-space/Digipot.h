/*
 * Digipot.h
 *
 *  Created on: 26 мая 2016 г.
 *      Author: developer
 */

#ifndef DIGIPOT_H_
#define DIGIPOT_H_

typedef enum {
	I2C_timeout = -2,		 // Операция I2C не уложилась в заданное время


};

// Создает на i2c шине событие START.
/*
 Если шина и так находится под нашим контролем - этот вызов создаст событие RESTART
 Возвращает 0 если все в порядке и не ноль в случае ошибки
*/
int i2c_start();


// Отправляет на шину 7мибитный адрес ведомого устройства с битом-флагом чтения/записи
/*
 Второй пареметр == 0 - обращение к ведомому в режиме записи..
 Возвращает 0 если все в порядке и не ноль в случае ошибки */
int i2c_send_slaw(uint8_t slave_addr, bool read_access);


// собирает и отправляет на шину конфигурационный байт
int configuration_byte();

// Создает на шине состояние STOP
/* Если возвращает не 0 - что-то пошло не так */
int i2c_stop();


#endif /* DIGIPOT_H_ */
