/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-11-30     RT-Thread    first version
 */

#include <rtthread.h>
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include "application.h"
#include"fal.h"

#define APP_VER "V1.5"
extern ota_flag ;

static int ota_app_vtor_reconfig(void){
#define NVIC_VTOR_MASK 0x3fffff80
    SCB->VTOR = RT_APP_PART_ADDR & NVIC_VTOR_MASK;   //修改中断向量表的跳转基地址

    return 0;
}
INIT_BOARD_EXPORT(ota_app_vtor_reconfig);


int main(void)
{

    fal_init();

    LOG_D("APP_VER: %s\n", APP_VER);

    rt_thread_mdelay(10*1000);
    if(ota_flag == 0){
        appStart();
    }


    rt_thread_mdelay(1000);
    return RT_EOK;
}


