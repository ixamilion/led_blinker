/*
 * led.c
 *
 *  Created on: Nov 5, 2020
 *      Author: Admin
 */

#include "leds/led.h"

#include "main.h"

const struct led_mode _mode_off =
{
	.func            = LF_CONSTANT,
	.time            = 0,
	.start_glow_rate = 0,
	.end_glow_rate   = 0
};


#define _MODES_IN_CFG   (3)
struct led_modes_cfg
{
	struct led_mode mode[_MODES_IN_CFG];
};

#define _MODES_CFG_CNT     (3)
static struct led_modes_cfg _user_modes[_MODES_CFG_CNT] =
{
	{
		.mode[0].func            = LF_LINEAR,
		.mode[0].time            = 100,
		.mode[0].start_glow_rate = 0,
		.mode[0].end_glow_rate   = 100,

		.mode[1].func            = LF_CONSTANT,
		.mode[1].time            = 1000,
		.mode[2].start_glow_rate = 100,
		.mode[1].end_glow_rate   = 100,

		.mode[2].func            = LF_LINEAR,
		.mode[2].time            = 100,
		.mode[2].start_glow_rate = 100,
		.mode[2].end_glow_rate   = 0,

	},
	{
		.mode[0].func            = LF_LINEAR,
		.mode[0].time            = 100,
		.mode[0].start_glow_rate = 0,
		.mode[0].end_glow_rate   = 20,

		.mode[1].func            = LF_CONSTANT,
		.mode[1].time            = 1000,
		.mode[1].start_glow_rate = 20,
		.mode[1].end_glow_rate   = 20,

		.mode[2].func            = LF_LINEAR,
		.mode[2].time            = 100,
		.mode[2].start_glow_rate = 20,
		.mode[2].end_glow_rate   = 0,

	},
	{
		.mode[0].func             = LF_LINEAR,
		.mode[0].time             = 1200,
		.mode[0].start_glow_rate  = 0,
		.mode[0].end_glow_rate    = 100,

		.mode[1].func             = LF_CONSTANT,
		.mode[1].time             = 100,
		.mode[1].start_glow_rate  = 100,
		.mode[1].end_glow_rate    = 100,

		.mode[2].func             = LF_LINEAR,
		.mode[2].time             = 5000,
		.mode[2].start_glow_rate  = 100,
		.mode[2].end_glow_rate    = 0,

	},
};

static uint8_t _curr_cfg_idx  = 0;
static uint8_t _curr_mode_idx = 0;

//
struct led_mode *led_get_next_mode(void)
{
	if (++_curr_mode_idx == _MODES_IN_CFG)
		_curr_mode_idx = 0;

	return &_user_modes[_curr_cfg_idx].mode[_curr_mode_idx];
}

//
void led_set_next_mode(void)
{
	if (++_curr_cfg_idx == _MODES_CFG_CNT)
		_curr_cfg_idx = 0;

	_curr_mode_idx = 0;

	bam_driver_set_mode(_user_modes[_curr_cfg_idx].mode);
}
