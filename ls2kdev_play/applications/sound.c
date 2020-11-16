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

#include <rtdevice.h>

#include "sound.h"

struct rt_device_pwm     *pwm_device = RT_NULL; //定义 pwm 设备指针

int sound_init(void)
{
    /* 查找PWM设备 */
    pwm_device = (struct rt_device_pwm *)rt_device_find(SOUND_PWM_DEVICE);
    if (pwm_device == RT_NULL)
    {
        return -RT_ERROR;
    }
    return 0;
}

int sound_on(void)
{
    rt_pwm_enable(pwm_device, SOUND_PWM_CH_1); //使能发声设备对应的 PWM 通道

    return 0;
}

int sound_off(void)
{
    rt_pwm_disable(pwm_device, SOUND_PWM_CH_1); //失能发声设备对应的 PWM 通道

    return 0;
}

int sound_set(rt_uint16_t freq, rt_uint8_t volume)
{
    rt_uint32_t period, pulse;

    if(freq == 0)
    {
        period = 1000000;
        pulse = 0;
        rt_pwm_set(pwm_device, SOUND_PWM_CH_1, period, pulse);//channel,period,pulse
    }  

    else
    {
        /* 将频率转化为周期 周期单位:ns 频率单位:HZ */
        period = 1000000000 / freq;  //unit:ns 1/HZ*10^9 = ns

        /* 根据声音大小计算占空比 蜂鸣器低电平触发 */
        pulse = period - period / 100 * volume;

        /* 利用 PWM API 设定 周期和占空比 */
        rt_pwm_set(pwm_device, SOUND_PWM_CH_1, period, pulse);//channel,period,pulse

    return 0;
    }
    
    
}