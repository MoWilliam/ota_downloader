/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-10-26     WangW       the first version
 */

//该模块主要是  开启串口读取和发送的线程操作

#include <rtthread.h>
#include "bsp/inc/b_prectrUART.h"
#include "inc/t_prectr_uart.h"

void task_thread_uart4_recv(void *ptr)
{
    rt_kprintf("t_thread_uart4_recv thread run\n");
    if(SD_NULL != ptr)
    {
        LPTaskObjectDef pstTaskObject = (LPTaskObjectDef)ptr;
        LPPreCtrFrameDef pstPreCtrObject = device_prectrl_object_get();
        while (pstTaskObject->brun_prectrUart_Recv)
        {

            PreCtrFrameDef dmf;
            bsp_uart_get(&dmf);
        }

        rt_kprintf("[TASK Module] uart4_recv thread exit\n");
        ut_thread_exit(pstTaskObject->Taskthread_prectrUart_Recv);
    }
}

void task_thread_uart4_send(void *ptr)
{
    rt_kprintf("t_thread_uart4_send thread run\n");
    if(SD_NULL != ptr)
    {
        LPTaskObjectDef pstTaskObject = (LPTaskObjectDef)ptr;
        LPPreCtrFrameDef pstPreCtrObject = device_prectrl_object_get();
        while (pstTaskObject->brun_prectrUart_Send)
        {

            PreCtrFrameDef dmf;
            bsp_uart_send(&dmf);

        }

        rt_kprintf("[TASK Module] uart4_send thread exit\n");
        ut_thread_exit(pstTaskObject->Taskthread_prectrUart_Send);
    }
}

void task_uart4_recv_init(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun_prectrUart_Recv = SD_FALSE;
}
void task_uart4_recv_start(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    if(SD_NULL != pstTaskObject)
    {
        if (SD_FALSE == pstTaskObject->brun_prectrUart_Recv)
        {
            pstTaskObject->brun_prectrUart_Recv = SD_TRUE;
            ut_thread_create(pstTaskObject->Taskthread_prectrUart_Recv,"TASK_UART4_RECV_THREAD",
                UT_THREAD_STACK_SIZE_LARGE,
                UT_THREAD_PRIORITY_DEFAULT,
                UT_THREAD_TICK_DEFAULT,
                task_thread_uart4_recv,pstTaskObject);

        }
    }
}

void task_uart4_recv_stop(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun_prectrUart_Recv = SD_FALSE;
}

void task_uart4_send_init(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun_prectrUart_Send = SD_FALSE;
}
void task_uart4_send_start(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    if(SD_NULL != pstTaskObject)
    {
        if (SD_FALSE == pstTaskObject->brun_prectrUart_Send)
        {
            pstTaskObject->brun_prectrUart_Send = SD_TRUE;
            ut_thread_create(pstTaskObject->Taskthread_prectrUart_Send,"TASK_UART4_SEND_THREAD",
                UT_THREAD_STACK_SIZE_LARGE,
                UT_THREAD_PRIORITY_DEFAULT,
                UT_THREAD_TICK_DEFAULT,
                task_thread_uart4_send,pstTaskObject);

        }
    }
}

void task_uart4_send_stop(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun_prectrUart_Send = SD_FALSE;
}
