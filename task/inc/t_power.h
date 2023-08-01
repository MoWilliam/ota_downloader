/*
 * @Author: wangwei wangwei@bitnei.com
 * @Date: 2023-04-28 10:33:37
 * @LastEditors: wangwei wangwei@bitnei.com
 * @LastEditTime: 2023-07-25 17:36:27
 * @FilePath: \compositesensor3\task\inc\t_power.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
#ifndef TASK_T_POWER_H_
#define TASK_T_POWER_H_

#include "application.h"
#include <rtthread.h>


void task_thread_esp8266sleep_manage(void *ptr);
void task_power_init(void);
void task_power_start(void);
void task_power_stop(void);

#endif /* TASK_T_POWER_H_ */
