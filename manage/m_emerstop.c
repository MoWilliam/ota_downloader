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



rt_uint8_t key_pressed_flags = 0; // 仅追踪一个按键状态的变量
rt_timer_t emerstop_timer = RT_NULL; // 定时器变量

// 读取按键状态
KEY_Action_TypeDef KEY_ReadPin(rt_base_t pin) {
    return (KEY_Action_TypeDef)rt_pin_read(pin);
}

// 设置按键状态的函数
void set_key_pressed_flags(int flag) {
    // 设置按键状态
    key_pressed_flags = flag;
}

// 获取按键状态的函数
rt_uint8_t get_key_pressed_flags(void) {
    return key_pressed_flags;
}

void emerstop_timer_callback(void *parameter) {
    // 读取按键状态
    KEY_Action_TypeDef curr_status = KEY_ReadPin(EmerStop_key);
    LPPreCtrRecvFrameDef pstPreCtrRecvFrameDef = device_prectrlrecv_object_get();
    if (curr_status == KEY_Action_Press) {
        set_key_pressed_flags(1);  // 按下状态，将状态置为1
        bsp_PreCtr_GPIO_stopall();
        pstPreCtrRecvFrameDef->m_stopFlag = 0;
        rt_kprintf("emerstop is start\n");
    } else {
        set_key_pressed_flags(0);  // 松开状态，将状态置为0
    }
}

void manage_emerstop_init(void) {
    rt_pin_mode(EmerStop_key, PIN_MODE_INPUT);

    // 创建定时器，定时周期为 1000 毫秒
    emerstop_timer = rt_timer_create("emerstop_timer", emerstop_timer_callback, RT_NULL, 1000, RT_TIMER_FLAG_PERIODIC);

    if (emerstop_timer != RT_NULL) {
        // 启动定时器
        rt_timer_start(emerstop_timer);
    } else {
        rt_kprintf("Failed to create emerstop timer\n");
    }
}




