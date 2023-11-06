/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-06-21     Wangwei       the first version
 */

#ifndef MANAGE_INC_M_EMERSTOP_H_
#define MANAGE_INC_M_EMERSTOP_H_

#include "application.h"
#include "bsp/inc/b_prectrgpio.h"

#define KEY_PRESSED_LEVEL      0   // 按键被按下时的电平
#define Debounce_delay         20 //消抖延时

// 按键动作，
typedef enum
{ 
	KEY_Action_Press = 0,  //按下
	KEY_Action_Release     //松开
}KEY_Action_TypeDef;


typedef enum TagPrectrEmerstopDef
{
    emPrectrEmerstop_FlagOff = 0,
    emUARTCmdStopPress_FlagOn

}enPrectrCmdDef;


void debounce_handler(void *param);
KEY_Action_TypeDef KEY_ReadPin(rt_base_t pin);

rt_uint8_t set_key_pressed_flags(int idx ,int flag);
rt_uint8_t get_key_pressed_flags(int idx);

#endif /* MANAGE_INC_M_EMERSTOP_H_ */
