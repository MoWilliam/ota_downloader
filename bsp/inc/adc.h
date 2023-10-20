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
#define ADC_CHANNAL 9

#define REG_VEL 3.3
#define CONVERT_BITS 12
#define SAMPLE_NUM 8

int Adc_Chargeread(void);
#endif
