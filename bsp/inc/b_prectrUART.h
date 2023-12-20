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
static rt_device_t serial_4;

#define UART_DEV_NAME "uart4"

#define ENDLESS_LOOP_TIMEOUT    20000
#define DEVICE_ID_LENGTH   20

#define ON                      1
#define OFF                     0



void bsp_uart_init(void);
void bsp_uart_get (PreCtrFrameDef* dmf);
//void bsp_uart_send(PreCtrFrameDef *dmf);
void bsp_uart_send();
#endif /* BSP_INC_B_PRECTRUARTT_H_ */
