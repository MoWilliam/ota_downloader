/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-06-5     Wangwei       the first version
 */

#ifndef BSP_INC_B_PRECTRGPIO_H_
#define BSP_INC_B_PRECTRGPIO_H_

#include "application.h"
#include "sys.h"
#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <drv_common.h>
#include <rtdbg.h>

#define pump1_Pin GPIO_PIN_2
#define pump1_GPIO_Port GPIOB
#define pump2_Pin GPIO_PIN_7
#define pump2_GPIO_Port GPIOE
#define pump3_Pin GPIO_PIN_8
#define pump3_GPIO_Port GPIOE
#define pump4_Pin GPIO_PIN_9
#define pump4_GPIO_Port GPIOE
#define pump5_Pin GPIO_PIN_10
#define pump5_GPIO_Port GPIOE
#define valve1_Pin GPIO_PIN_11
#define valve1_GPIO_Port GPIOE
#define valve2_Pin GPIO_PIN_12
#define valve2_GPIO_Port GPIOE
#define valve3_Pin GPIO_PIN_13
#define valve3_GPIO_Port GPIOE
#define valve4_Pin GPIO_PIN_14
#define valve4_GPIO_Port GPIOE
#define valve5_Pin GPIO_PIN_15
#define valve5_GPIO_Port GPIOE


#define NUM_PINS 5
#define EmerStop_key1             GET_PIN(D, 8)   //急停按钮的引脚编号
#define EmerStop_key2             GET_PIN(D, 9)   
#define EmerStop_key3             GET_PIN(D, 10)   
#define EmerStop_key4             GET_PIN(D, 11)   
#define EmerStop_key5             GET_PIN(D, 12) 


#define presorid_1     1
#define presorid_2     2
#define presorid_3     3
#define presorid_4     4
#define presorid_5     5
//#define MAX_KEY_NUM         5


/*#define PRECTR_THREAD_PRESOR_ON       1
#define PRECTR_THREAD_PRESOR_OFF      0

#define NO_KEY_PRESS       0

#define emerstop_THREAD_SIG        1*/





void bsp_PreCtr_GPIO_Init(void);
void bsp_PreCtr_GPIO_start(int device_id);
void bsp_PreCtr_GPIO_stop(int device_id);


#endif /* BSP_INC_B_B_PRECTRGPIO_H_ */
