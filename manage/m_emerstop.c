/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-06-21     WangWei       the first version
 */

//进行按键的中断控制响应（每个按键与相应压力采集器进行对应）
//需要写一个信号量的闭锁情况（按键按下，闭锁标志位置1，所对应传感器设备无法进行充气 ，按键松开，标志位置0，可以进行充气操作）

#include <rtthread.h>
#include <rtdevice.h>
#include "bsp/inc/b_prectrgpio.h"
#include "inc/m_device.h"
#include "inc/m_emerstop.h"


rt_base_t pins[NUM_PINS] = {EmerStop_key1, EmerStop_key2, EmerStop_key3, EmerStop_key4, EmerStop_key5};
rt_uint8_t key_pressed_flags[NUM_PINS] = {0};    //储存每个急停按钮的状态标志位，松开后状态位置0

// 读取按键状态
KEY_Action_TypeDef KEY_ReadPin(rt_base_t pin)   //返回结果按下或者松开
{
    return (KEY_Action_TypeDef)rt_pin_read(pin);
}

void debounce_handler(void *param)  //急停按键的处理
{
	rt_base_t pin = (rt_base_t)param;
	KEY_Action_TypeDef status = KEY_ReadPin(pin);
	int key_index = -1;

	// 根据急停按钮的引脚找到对应的索引
    for (int i = 0; i < NUM_PINS; i++)
    {
        if (pin == pins[i])
        {
            key_index = i;
            break;
        }
    }
    
    if (key_index != -1)
    {
        // 读取对应按键状态标志位
        if (status == KEY_Action_Press)
        {
			set_key_pressed_flags(key_index,1);  // 急停按键按下，对应急停标志位设为1

        }
        else if (status == KEY_Action_Release)
        {
			set_key_pressed_flags(key_index,0);  // 急停按键松开，对应急停标志位设为0

        }
    }
    
    // 在这里对按键状态进行处理，判断按键是否按下，执行相应操作
    if (status == KEY_Action_Press)
    {
        if (pin == EmerStop_key1)
        {
            bsp_PreCtr_GPIO_stop(PressureSensor1);
        }
        else if (pin == EmerStop_key2)
        {
            bsp_PreCtr_GPIO_stop(PressureSensor2);
        }
        else if (pin == EmerStop_key3)
        {
            bsp_PreCtr_GPIO_stop(PressureSensor3);
        }
        else if (pin == EmerStop_key4)
        {
            bsp_PreCtr_GPIO_stop(PressureSensor4);
        }
        else if (pin == EmerStop_key5)
        {
            bsp_PreCtr_GPIO_stop(PressureSensor5);
        }
    }
}

//循环读取引脚电平状态
void read_pins_status(void)
{
	for(int i=0; i < NUM_PINS; i++)
	{
		rt_base_t pin = pins[i];
		rt_thread_delay(Debounce_delay); //消抖延时

		//读取引脚状态，如果状态变化，则启动定时器进行延时消抖处理
		KEY_Action_TypeDef prev_status = KEY_ReadPin(pin);
		rt_thread_delay(Debounce_delay); //消抖延时
		KEY_Action_TypeDef curr_status = KEY_ReadPin(pin);
		if(prev_status != curr_status)
		{
			//启动定时器进行延时消抖处理
			rt_time_t timer = rt_timer_create("debounce", debounce_handler, 
												(void*)pin, Debounce_delay, RT_TIMER_FLAG_PERIODIC);
			rt_timer_start(timer);
		}
	}
} 


rt_uint8_t set_key_pressed_flags(int idx ,int flag)
{
	// 加锁
	key_pressed_flags[idx] = flag;
}

rt_uint8_t get_key_pressed_flags(int idx)
{
	// 加锁
	if ( idx >=0 && idx < NUM_PINS){  //限制idx的条件范围
		return key_pressed_flags[idx];
	}
	return 0;
}


void manage_emerstop_init(void)  //开启循环扫描，检测急停按键的按键状态
{
	read_pins_status();
}




