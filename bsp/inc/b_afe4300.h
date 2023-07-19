/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
#ifndef BSP_B_AFE4300_H_
#define BSP_B_AFE4300_H_

#include "application.h"

float kalman_filter(float inData);
void bsp_afe4300_init(void);
void bsp_afe4300_get(BioFrameDef* dmf);

#endif /* BSP_B_AFE4300_H_ */
