/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-06-02     WangW       the first version
 */

/**
该模块主要是  读取主控终端发送到的数据，并进行操作命令（app上 设备id、加压和泄压操作）  app以外急停命令

**/

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "bsp/inc/b_prectrgpio.h"
#include "bsp/inc/b_prectrUART.h"
#include "inc/m_prectr.h"
#include "inc/m_emerstop.h"


void prectr_recv_thread_entry(void *ptr)    //线程任务
{
    rt_err_t uartRecvRet = RT_EOK;
    //char deviceid[DEVICE_ID_LENGTH];  //字符串类型
    //rt_uint8_t cmdtype = 0;   //0，1类型
    rt_uint8_t flag = 0;      //用来存储急停按键的按键状态
    if(SD_NULL != ptr)
    {   

        LPAppObjectDef pstAppObject = (LPAppObjectDef)ptr;
        while(pstAppObject->brun )
        {
            uartRecvRet = rt_sem_take(&rx_sem_4, RT_WAITING_FOREVER);  // 获取USART4接收信号量，有返回的状态
            PreCtrFrameDef dmf;
            bsp_uart_send(&dmf);
            if (RT_EOK == uartRecvRet)
            {

                // 根据设备ID选择设备，并根据命令类型确定设备的开关状态
                if (strcmp(dmf.m_pressureid, PressureSensor1) == 0)
                {
                    flag = get_key_pressed_flags(0);
                    if (dmf.m_cmdType == PRESOR_ON && flag == 0)  //命令状态为press_on且急停按钮状态标志位为0，才能开启
                    {
                        bsp_PreCtr_GPIO_start(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ start PressureSensor1\n");   //添加输出打印
                    }
                    else if (dmf.m_cmdType == PRESOR_OFF || flag == 1)
                    {
                        bsp_PreCtr_GPIO_stop(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ stop PressureSensor1\n");   //添加输出打印
                    }
                }
                else if (strcmp(dmf.m_pressureid, PressureSensor2) == 0)
                {
                    flag = get_key_pressed_flags(1);
                    if (dmf.m_cmdType == PRESOR_ON && flag == 0)
                    {
                        bsp_PreCtr_GPIO_start(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ start PressureSensor2\n");   //添加输出打印
                    }
                    else if (dmf.m_cmdType == PRESOR_OFF || flag == 1)
                    {
                        bsp_PreCtr_GPIO_stop(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ stop PressureSensor2\n");   //添加输出打印
                    }
                }
                else if (strcmp(dmf.m_pressureid, PressureSensor3) == 0)
                {
                    flag = get_key_pressed_flags(2);
                    if (dmf.m_cmdType == PRESOR_ON && flag == 0)
                    {
                        bsp_PreCtr_GPIO_start(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ start PressureSensor3\n");   //添加输出打印
                    }
                    else if (dmf.m_cmdType == PRESOR_OFF || flag == 1)
                    {
                        bsp_PreCtr_GPIO_stop(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ stop PressureSensor3\n");   //添加输出打印
                    }
                }
                else if (strcmp(dmf.m_pressureid, PressureSensor4) == 0)
                {
                    flag = get_key_pressed_flags(3);
                    if (dmf.m_cmdType == PRESOR_ON && flag == 0)
                    {
                        bsp_PreCtr_GPIO_start(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ start PressureSensor4\n");   //添加输出打印
                    }
                    else if (dmf.m_cmdType == PRESOR_OFF || flag == 1)
                    {
                        bsp_PreCtr_GPIO_stop(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ stop PressureSensor4\n");   //添加输出打印
                    }
                }
                else if (strcmp(dmf.m_pressureid, PressureSensor5) == 0)
                {
                    flag = get_key_pressed_flags(4);
                    if (dmf.m_cmdType == PRESOR_ON && flag == 0)
                    {  
                        bsp_PreCtr_GPIO_start(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ start PressureSensor5\n");   //添加输出打印
                    }
                    else if (dmf.m_cmdType == PRESOR_OFF || flag == 1)
                    {
                        bsp_PreCtr_GPIO_stop(dmf.m_pressureid);
                        rt_kprintf("  /m_prectr.c/ stop PressureSensor5\n");   //添加输出打印
                    }
                }

                memset(g_prectrUart_RxBuf, 0, UART_BUFFER_SIZE);  // 清空接收缓冲区
            }
            rt_thread_mdelay(100);
        }
    }
}

//uart_get 和uart_send程序的编写
void m_prectruart_thread(void)  //开启线程，UART信号的接收
{
    {
        LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
        if(SD_NULL != pstPressControlObject)
        {
            if ( pstPressControlObject->brun_prectrUart == SD_FALSE)
            {
                pstPressControlObject->brun_prectrUart = SD_TRUE;
                ut_thread_create(pstPressControlObject->Thead_prectrUart,"UARTRecv_PRECTR_THREAD",
                                            UT_THREAD_STACK_SIZE_LARGE,
                                            UT_THREAD_PRIORITY_DEFAULT,
                                            UT_THREAD_TICK_DEFAULT,
                                            bsp_uart_send,pstPressControlObject);  //uart发送数据的线程
            }
            //rt_kprintf("  /m_prectr.c/ m_prectruart_thread run\n");   //添加输出打印

        }
    }
}

void m_prectr_thread(void)    
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    if(SD_NULL != pstPressControlObject)
    {
        if ( pstPressControlObject->brun_prectrRecv == SD_FALSE)
        {
            pstPressControlObject->brun_prectrRecv = SD_TRUE;
            ut_thread_create(pstPressControlObject->Thead_prectrRecv,"PRECTR_THREAD",
                            UT_THREAD_STACK_SIZE_LARGE,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_DEFAULT,
                            prectr_recv_thread_entry,pstPressControlObject);
        }
        //rt_kprintf("  /m_prectr.c/ m_prectr_thread run\n");   //添加输出打印

    }
}


void manage_prectr_init(void)   
{
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    pstAppObject = SD_NULL;
    
}

void manage_prectr_start(void)   
{

    m_prectr_thread();
}

void manage_prectr_stop(void)   
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_prectrRecv = SD_FALSE;

}



void manage_prectruart_start(void)
{

    m_prectruart_thread();
}

void manage_prectruart_stop(void)
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_prectrUart = SD_FALSE;

}


