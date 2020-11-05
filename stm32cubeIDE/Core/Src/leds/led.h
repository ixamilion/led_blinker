/*
 * led.h
 *
 *  Created on: Nov 4, 2020
 *      Author: Admin
 */

#ifndef SRC_LEDS_LED_H_
#define SRC_LEDS_LED_H_

#include "main.h"
#include "stdio.h"

// Функция изменения яркости
enum led_func
{
	LF_UNDEFINED,               // Не определена (не задана)
	LF_CONSTANT,                // Мгновенное изменение до заданного уровня
	LF_LINEAR                   // Линейная функция изменения
	//LF_EXPONENT               // (не реализовано) Экспоненциальнае функция изменения
};

//
struct led_mode
{
	enum led_func func;              // Функция изменения
	uint32_t      time;              // Время исполнения
	uint8_t       start_glow_rate;   // Начальная яркость, % [0..100]
	uint8_t       end_glow_rate;     // Конечная яркость,  % [0..100]
};

//
extern const struct led_mode _mode_off;

// Светодиод
struct led
{
	GPIO_TypeDef  *port;
	uint16_t       pin;

	struct led_mode *mode;           // Режим работы светодиода
	uint32_t       downcounter;      // Оставшееся время команды
	uint8_t        frame_glow_lvl;   // Текущий расчетный уровень яркости
};

//
struct led_mode *led_get_next_mode(void);


void led_set_next_mode(void);

#endif /* SRC_LEDS_LED_H_ */
