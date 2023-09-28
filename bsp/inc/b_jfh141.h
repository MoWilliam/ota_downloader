/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-13     ZHZH       the first version
 */
#ifndef BSP_INC_B_JFH141_H_
#define BSP_INC_B_JFH141_H_
#include "application.h"

extern float kalman_filter_jfh141(float inData);
void bsp_jfh141_init(void);
//void bsp_jfh141_get(Spo2FrameDef* dmf);
void bsp_jfh141_get(SensorDataFrameDef* dmf); //2023.9.27

#endif /* BSP_INC_B_JFH141_H_ */
