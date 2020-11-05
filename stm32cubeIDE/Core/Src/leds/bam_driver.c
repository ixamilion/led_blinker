/*
 * bam_driver.c
 *
 *  Created on: Nov 3, 2020
 *      Author: Admin
 */

#include "leds/bam_driver.h"


// Для кодирования уровня яркости возьмем 1байт
// Поучим 256 цветов за 8 прерываний таймера
// Длительность каждого прерывания, в мс
static const uint16_t tim_reload[8] = {39, 78, 156, 312, 626, 1250, 2500, 5000};

// Структура драйвера
struct BAM_driver
{
	// Управляемый светодиод
	struct led led;

	struct {
#define _STEP_HIGHER_IDX                                   (7)
		uint8_t step_idx :3;       // Индекс прерывания [0..7]
		uint8_t _unused  :5;
	};
};
static struct BAM_driver _bam;
//static struct led_mode _mode_off =
//{
//	{.func = LF_CONSTANT, .glow_rate = 0, .time = 0, .next = 0}
//};

static void _update_frame(void);

// Таймер, организующий прерывания в кадре
extern TIM_HandleTypeDef htim2;

// Обработчик очередного прерывания
void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim2);

	htim2.Instance->ARR = tim_reload[_bam.step_idx];
	HAL_TIM_Base_Start_IT(&htim2);

	if (_bam.led.frame_glow_lvl & (1 << _bam.step_idx)) {
		HAL_GPIO_WritePin(_bam.led.port, _bam.led.pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(_bam.led.port, _bam.led.pin, GPIO_PIN_RESET);
	}

	if (_bam.step_idx == _STEP_HIGHER_IDX) {
		// Готовим новый кадр
		_update_frame();
	}

	_bam.step_idx--;
}



//
void bam_driver_init(void)
{
	// Настройка светодиода
	_bam.led.port = LD3_GPIO_Port;
	_bam.led.pin  = LD3_Pin;
	_bam.led.frame_glow_lvl = 0;
	// Сейчас происходит в main::MX_GPIO_Init()

	// Настройка таймера
	// Сейчас происходит в main::MX_TIM2_Init()
}

//
void bam_driver_start(void)
{
	// Стартуем выключив светодиод
	bam_driver_set_mode(&_mode_off);
}

//
void bam_driver_set_mode(struct led_mode *mode)
{
	// Новая яркость - с нового кадра
	HAL_TIM_Base_Stop_IT(&htim2);

	if (mode == 0)
		return;

	_bam.step_idx           = _STEP_HIGHER_IDX;
	htim2.Instance->ARR     = tim_reload[_bam.step_idx];
	_bam.led.mode = mode;
	_bam.led.downcounter    = _bam.led.mode->time;
	// Округляем счетчик до кратного длительности окна
	_bam.led.downcounter   /= BAM_FRAME_PERIOD_MS;
	_bam.led.downcounter   *= BAM_FRAME_PERIOD_MS;

	HAL_TIM_Base_Start_IT(&htim2);
}


static void _update_frame(void)
{
	struct led *l = &_bam.led;
	uint8_t  glow = 0x00;

	l->downcounter -= BAM_FRAME_PERIOD_MS;
	if (l->downcounter == 0) {
		l->mode = led_get_next_mode();

		l->downcounter    = l->mode->time;
		// Округляем счетчик до кратного длительности окна
		l->downcounter   /= BAM_FRAME_PERIOD_MS;
		l->downcounter   *= BAM_FRAME_PERIOD_MS;
	}

	switch (l->mode->func) {
	case LF_CONSTANT:
		glow = (uint8_t)((l->mode->end_glow_rate * 255) / 100);
		break;

	case LF_LINEAR: {
		uint32_t k = 255;
		k *= (l->mode->time - l->downcounter);
		k /= l->mode->time;

		if (l->mode->start_glow_rate < l->mode->end_glow_rate) {
			glow = l->mode->start_glow_rate * 255 / 100;
			glow += (uint8_t)(((l->mode->end_glow_rate - l->mode->start_glow_rate) * k) / 100);
		} else {
			glow = l->mode->start_glow_rate * 255 / 100;
			glow -= (uint8_t)(((l->mode->start_glow_rate - l->mode->end_glow_rate) * k) / 100);
		}

		break;
	}

	case LF_UNDEFINED:
	default:
		l->mode->func= LF_CONSTANT;
		l->mode->end_glow_rate = 0;
	}

	l->frame_glow_lvl = glow;
}
