/*
 * @Author: wangwei wangwei@bitnei.com
 * @Date: 2023-04-28 10:33:19
 * @LastEditors: wangwei wangwei@bitnei.com
 * @LastEditTime: 2023-07-26 14:29:13
 * @FilePath: \compositesensor3\bsp\inc\b_afe4300.h
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
#ifndef BSP_B_AFE4300_H_
#define BSP_B_AFE4300_H_

#include "application.h"

float kalman_filter(float inData);
void bsp_afe4300_init(void);
void bsp_afe4300_get(BioFrameDef* dmf);

#endif /* BSP_B_AFE4300_H_ */
