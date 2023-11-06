/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-06-20 13:51:08
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-06-21 19:58:17
 * @FilePath: \pressurecontrolsensor\compositesensor\manage\inc\m_prectr.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-06-02     Wangwei       the first version
 */
#ifndef M_PRECTR_H_
#define M_PRECTR_H_

#include "application.h"

#define UART_BUFFER_SIZE        256
uint8_t  g_prectrUart_RxBuf[UART_BUFFER_SIZE];

//#define PRESOR_ON       1
//#define PRESOR_OFF      2

void manage_prectr_init(void) ; 
void manage_prectr_start(void);  
void manage_prectr_stop(void);

  
#endif /* M_PRECTR_H_ */
