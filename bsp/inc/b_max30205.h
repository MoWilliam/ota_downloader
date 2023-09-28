/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-15     ZHZH       the first version
 */
#ifndef BSP_INC_B_MAX30205_H_
#define BSP_INC_B_MAX30205_H_


#include "application.h"

extern float kalman_filter_temp(float inData);
void bsp_max30205_init(void);
// void bsp_max30205_get(DataFrameDef* dmf);
void bsp_max30205_get(SensorDataFrameDef* dmf); //2023.9.27


#endif /* BSP_INC_B_MAX30205_H_ */
