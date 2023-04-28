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

//1维的卡尔曼滤波
typedef struct {
    float x;  // state 
    float A;  // x(n)=A*x(n-1)+u(n),u(n)~N(0,q) 
    float H;  // z(n)=H*x(n)+w(n),w(n)~N(0,r)   
    float q;  // process(predict) noise convariance 协方差
    float r;  // measure noise convariance 
    float p;  // estimated error convariance 估计误差协方差
    float gain;
}kalman_struct;

void kalman_init(kalman_struct *kalman_lcw, float init_x, float init_p);
float kalman_filter(kalman_struct *kalman_lcw, float z_measure);

void bsp_afe4300_init(void);
void bsp_afe4300_get(BioFrameDef* dmf);

#endif /* BSP_B_AFE4300_H_ */
