/*
 * bam_driver.c
 *
 *  Created on: Nov 3, 2020
 *      Author: Admin
 */

#include "leds/bam_driver.h"

#include "main.h"

// Для кодирования уровня яркости возьмем 1байт
// Поучим 256 цветов за 8 прерываний таймера
// Длительность каждого прерывания, в мс
static const uint16_t tim_reload[8] = {39, 78, 156, 312, 626, 1250, 2500, 5000};

// Структура драйвера
struct BAM_driver
{
	// Управляемый светодиод
	GPIO_TypeDef  *led_port;
	uint16_t      led_pin;

	uint8_t glow_level;            // Уровень текущей яркости [0..255]

	struct {
#define _STEP_HIGHER_IDX                                   (7)
		uint8_t step_idx :3;       // Индекс прерывания [0..7]
		uint8_t _unused  :5;
	};
};
static struct BAM_driver _bam;

// Таймер, организующий прерывания в кадре
extern TIM_HandleTypeDef htim2;

// Обработчик очередного прерывания
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);

	htim2.Instance->ARR = tim_reload[_bam.step_idx];
	HAL_TIM_Base_Start_IT(&htim2);

	if (_bam.glow_level & (1 << _bam.step_idx)) {
		HAL_GPIO_WritePin(_bam.led_port, _bam.led_pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(_bam.led_port, _bam.led_pin, GPIO_PIN_RESET);
	}

	if (_bam.step_idx == _STEP_HIGHER_IDX) {
		// Готовим новый кадр
	}

	_bam.step_idx--;
}



//
void bam_driver_init(void)
{
	// Настройка светодиода
	_bam.led_port = LD3_GPIO_Port;
	_bam.led_pin  = LD3_Pin;
	// Сейчас происходит в main::MX_GPIO_Init()

	// Настройка таймера
	// Сейчас происходит в main::MX_TIM2_Init()
}

//
void bam_driver_start(void)
{
	// Стартуем выключив светодиод
	bam_driver_set_glow_level(100);
}

//
void bam_driver_set_glow_level(uint8_t lvl)
{
	// Новая яркость - с нового кадра
	HAL_TIM_Base_Stop_IT(&htim2);
	_bam.glow_level = lvl;
	_bam.step_idx   = _STEP_HIGHER_IDX;
	htim2.Instance->ARR = tim_reload[_bam.step_idx];
	HAL_TIM_Base_Start_IT(&htim2);
}
