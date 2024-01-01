/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-26     wangwei       the first version
 */
#ifndef TASK_INC_T_PRECTR_UART_H_
#define TASK_INC_T_PRECTR_UART_H_

#include "application.h"

void task_uart4_recv_init(void);
void task_uart4_recv_start(void);
void task_uart4_recv_stop(void);

 void task_uart4_send_init(void);
 void task_uart4_send_start(void);
 void task_uart4_send_stop(void);


#endif /* TASK_INC_T_PRECTR_UART_H_ */
