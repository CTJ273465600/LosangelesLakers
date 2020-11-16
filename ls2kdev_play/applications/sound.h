/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2018-10-17     flybreak     the first version
* 2020-10-12     anyang       using for loongxin2k1000
*/

#ifndef SOUND_H
#define SOUND_H

#include <rtthread.h>
//#include <stdint.h>


#define SOUND_PWM_DEVICE  "pwm0"
#define SOUND_PWM_CH_1     1
#define SOUND_PWM_CH_2     2
#define SOUND_PWM_CH_3     3


int sound_init(void);                               //发声模块初始化
int sound_on(void);                                 //发声模块开
int sound_off(void);                                //发声模块关
int sound_set(rt_uint16_t freq, rt_uint8_t volume); //发声模块设定

#endif