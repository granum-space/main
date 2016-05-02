/*
 * i2c.h
 *
 * Работа с шиной I2C
 *
 * Сама шина хорошо описана тут:
 * http://easyelectronics.ru/interface-bus-iic-i2c.html
 *
 * Описание модуля I2C микроконтроллеров AVR в переведенном даташите - тут
 * http://www.gaw.ru/html.cgi/txt/doc/micros/avr/arh128/18.htm
 *
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Ошибки шины I2C
typedef enum {
	I2C_ERROR_NO_ACK = -1, // ведомый не подтвердил получение байта
} i2c_error_value_t;


// Инициализация i2c.
void i2c_init();

// Создает на i2c шине событие START.
/*
 Если шина и так находится под нашим контролем - этот вызов создаст событие RESTART
 Возвращает 0 если все в порядке и одно из значений i2c_error_value_t в случае ошибки
*/
int i2c_start();

// Создает на шине состояние STOP
/* Если возвращает не 0 - что-то пошло не так */
int i2c_stop();


// Отправляет на шину 7мибитный адрес ведомого устройства с битом-флагом чтения/записи
/*
 Второй пареметр == 0 - обращение к ведомому в режиме записи.
 Второй параметр != 0 - обращение к ведомому в режиме чтения.
 Возвращает 0 если все в порядке и одно из значений i2c_error_value_t в случае ошибки
*/
int i2c_send_slaw(uint8_t slave_addr, bool read_access);

// Передача данных ведомому устройству
/*
 Перед вызовом этой функции на шину нужно передать адрес ведомого без бита чтения - `i2c_send_slaw(..., 0);`

 Записывает data_size байт из памяти по указателю data_ptr на устройство.
 Возвращает 0 если все в порядке и одно из значений i2c_error_value_t в случае ошибки
*/
int i2c_write(const void * data_ptr, size_t data_size);


// Получение данных от ведомого устройства
/*
 Перед вызовом этой функции на шину нужно передать адрес ведомого без бита чтения - `i2c_send_slaw(..., 0);`

 Получает data_size байт от ведомого устройства и записывает по указанному указателю data_ptr.
 аргумент NACK_at_end позволяет управлять подтверждением получения байта от ведомого.
 если этот аргумент установлен в true, то после получения последнего байта ведомому будет передан
 Not ACKnowledge, что означает завершение сеанса чтения.
 Возвращает 0 если все в порядке и одно из значений i2c_error_value_t в случае ошибки.
*/
int i2c_read(void * data_ptr, size_t data_size, bool NACK_at_end);

#endif /* I2C_H_ */