/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-06-5     Wangwei       the first version
 */
#ifndef BSP_INC_B_PRECTRUART_H_
#define BSP_INC_B_PRECTRUART_H_

#include "application.h"

// 用于接收消息的信号量
static struct rt_semaphore rx_sem_4;
static rt_device_t serial;

#define PreCtr_UART_NAME        "uart4"

#define ENDLESS_LOOP_TIMEOUT	20000

#define DEVICE_ID_LENGTH   20




#define ON                      1
#define OFF                     0



#define ID_PreSor1                 1
#define ID_PreSor2                 2
#define ID_PreSor3                 3
#define ID_PreSor4                 4
#define ID_PreSor5                 5

void bsp_uart_init(void);
void bsp_uart_get(PreCtrmqFrameDef* dmf);

#endif /* BSP_INC_B_PRECTRUARTT_H_ */
