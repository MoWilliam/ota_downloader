/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
#ifndef TASK_T_MONITOR_H_
#define TASK_T_MONITOR_H_

#include "application.h"

void task_monitor_init(void);
void task_monitor_start(void);
void task_monitor_stop(void);

#endif /* TASK_T_MONITOR_H_ */
