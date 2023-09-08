/*
 * @Author: wangwei wangwei@bitnei.com
 * @Date: 2023-06-28 09:26:51
 * @LastEditors: wangwei wangwei@bitnei.com
 * @LastEditTime: 2023-07-21 14:51:24
 * @FilePath: \compositesensor3\bsp\inc\b_prectrgpio.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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

//pump-pin
#define pump1_Pin GPIO_PIN_12
#define pump1_GPIO_Port GPIOB
#define pump2_Pin GPIO_PIN_13
#define pump2_GPIO_Port GPIOB
#define pump3_Pin GPIO_PIN_14
#define pump3_GPIO_Port GPIOB
#define pump4_Pin GPIO_PIN_15
#define pump4_GPIO_Port GPIOB
#define pump5_Pin GPIO_PIN_8
#define pump5_GPIO_Port GPIOD

//valve-pin
#define valve1_Pin GPIO_PIN_2
#define valve1_GPIO_Port GPIOE
#define valve2_Pin GPIO_PIN_3
#define valve2_GPIO_Port GPIOE
#define valve3_Pin GPIO_PIN_4
#define valve3_GPIO_Port GPIOE
#define valve4_Pin GPIO_PIN_5
#define valve4_GPIO_Port GPIOE
#define valve5_Pin GPIO_PIN_6
#define valve5_GPIO_Port GPIOE


#define NUM_PINS 5
#define EmerStop_key1             GET_PIN(D, 9)   //急停按钮的引脚编号
#define EmerStop_key2             GET_PIN(D, 10)   
#define EmerStop_key3             GET_PIN(D, 11)   
#define EmerStop_key4             GET_PIN(D, 12)   
#define EmerStop_key5             GET_PIN(D, 13) 


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
