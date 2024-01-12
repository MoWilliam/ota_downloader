/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-17     Wangwei       the first version
 */

#ifndef __ADC_H
#define __ADC_H

#include "application.h"
#include "sys.h"

#define ADC_DEV_NAME "adc1"
#define ADC_CHANNAL 5   //ADC通道
#define REFER_VOLTAGE       330
#define CONVERT_BITS        (1 << 12)   // 转换位数为12位


rt_uint32_t bsp_voltage_get(SensorDataFrameDef* dmf); 
#endif
