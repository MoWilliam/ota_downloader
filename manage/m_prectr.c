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
    rt_uint8_t flag = 0;      //用来存储急停按键的按键状态
    if(SD_NULL != ptr)
    {   

        LPPressControlObjectDef pstPressControlObject = (LPPressControlObjectDef)ptr;
        LPPreCtrFrameDef pstPreCtrFrameDef = device_prectrl_object_get();
        while(pstPressControlObject->brun_preControl )
        {
            //PreCtrFrameDef dmf;
            //bsp_uart_get(&dmf);
            //rt_kprintf("***222***\n");
            // 根据设备ID选择设备，并根据命令类型确定设备的开关状态
            if (strcmp(pstPreCtrFrameDef->m_pressureid, PressureSensor1) == 0)
            {
                flag = get_key_pressed_flags(0);
                if (pstPreCtrFrameDef->m_cmdType == PRESOR_ON && flag == 0)  //命令状态为press_on且急停按钮状态标志位为0，才能开启
                {
                    bsp_PreCtr_GPIO_start(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ start PressureSensor1\n");   //添加输出打印
                }
                else if (pstPreCtrFrameDef->m_cmdType == PRESOR_OFF || flag == 1)
                {
                    bsp_PreCtr_GPIO_stop(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ stop PressureSensor1\n");   //添加输出打印
                }
            }
            else if (strcmp(pstPreCtrFrameDef->m_pressureid, PressureSensor2) == 0)
            {
                flag = get_key_pressed_flags(1);
                if (pstPreCtrFrameDef->m_cmdType == PRESOR_ON && flag == 0)
                {
                    bsp_PreCtr_GPIO_start(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ start PressureSensor2\n");   //添加输出打印
                }
                else if (pstPreCtrFrameDef->m_cmdType == PRESOR_OFF || flag == 1)
                {
                    bsp_PreCtr_GPIO_stop(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ stop PressureSensor2\n");   //添加输出打印
                }
            }
            else if (strcmp(pstPreCtrFrameDef->m_pressureid, PressureSensor3) == 0)
            {
                flag = get_key_pressed_flags(2);
                if (pstPreCtrFrameDef->m_cmdType == PRESOR_ON && flag == 0)
                {
                    bsp_PreCtr_GPIO_start(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ start PressureSensor3\n");   //添加输出打印
                }
                else if (pstPreCtrFrameDef->m_cmdType == PRESOR_OFF || flag == 1)
                {
                    bsp_PreCtr_GPIO_stop(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ stop PressureSensor3\n");   //添加输出打印
                }
            }
            else if (strcmp(pstPreCtrFrameDef->m_pressureid, PressureSensor4) == 0)
            {
                flag = get_key_pressed_flags(3);
                if (pstPreCtrFrameDef->m_cmdType == PRESOR_ON && flag == 0)
                {
                    bsp_PreCtr_GPIO_start(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ start PressureSensor4\n");   //添加输出打印
                }
                else if (pstPreCtrFrameDef->m_cmdType == PRESOR_OFF || flag == 1)
                {
                    bsp_PreCtr_GPIO_stop(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ stop PressureSensor4\n");   //添加输出打印
                }
            }
            else if (strcmp(pstPreCtrFrameDef->m_pressureid, PressureSensor5) == 0)
            {
                flag = get_key_pressed_flags(4);
                if (pstPreCtrFrameDef->m_cmdType == PRESOR_ON && flag == 0)
                {  
                    bsp_PreCtr_GPIO_start(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ start PressureSensor5\n");   //添加输出打印
                }
                else if (pstPreCtrFrameDef->m_cmdType == PRESOR_OFF || flag == 1)
                {
                    bsp_PreCtr_GPIO_stop(pstPreCtrFrameDef->m_pressureid);
                    rt_kprintf("  /m_prectr.c/ stop PressureSensor5\n");   //添加输出打印
                }
            }

            memset(g_prectrUart_RxBuf, 0, UART_BUFFER_SIZE);  // 清空接收缓冲区
            rt_thread_mdelay(1000);
        }
        
    }
}

void m_thread_uart4_recv(void *ptr)
{
    rt_kprintf("m_thread_uart4_recv thread run\n");
    if(SD_NULL != ptr)
    {
        LPPressControlObjectDef pstPressControlObject = (LPPressControlObjectDef)ptr;
        LPPreCtrFrameDef pstPreCtrObject = device_prectrl_object_get();
        while (pstPressControlObject->brun_prectrUart)
        {
            

            bsp_uart_get(pstPressControlObject);
            //rt_thread_mdelay(1000);
        }

        rt_kprintf("[Manage Module] uart4_recv thread exit\n");
        ut_thread_exit(pstPressControlObject->Thead_prectrUart);
    }
}



//压力控制命令线程
void m_prectr_thread(void)    
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    if(SD_NULL != pstPressControlObject)
    {
        if (  pstPressControlObject->brun_preControl == SD_FALSE)
        {
            pstPressControlObject->brun_preControl = SD_TRUE;
            ut_thread_create(pstPressControlObject->Thead_preControl,"PRECTRControl_THREAD",
                            UT_THREAD_STACK_SIZE_LARGE,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_DEFAULT,
                            prectr_recv_thread_entry,pstPressControlObject);
        }
        rt_kprintf("  /m_prectr.c/ m_preControl_thread run\n");   //添加输出打印

    }
}



void m_uart4_recv_init(void)
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_prectrUart = SD_FALSE;
}
void m_uart4_recv_start(void)
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    if(SD_NULL != pstPressControlObject)
    {
        if (SD_FALSE == pstPressControlObject->brun_prectrUart)
        {
            pstPressControlObject->brun_prectrUart = SD_TRUE;
            ut_thread_create(pstPressControlObject->Thead_prectrUart,"MANAGE_UART4_RECV_THREAD",
                UT_THREAD_STACK_SIZE_LARGE,
                UT_THREAD_PRIORITY_DEFAULT,
                UT_THREAD_TICK_DEFAULT,
                m_thread_uart4_recv,pstPressControlObject);

        }
    }
}

void m_uart4_recv_stop(void)
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_prectrUart = SD_FALSE;
}

void manage_prectr_init(void)   
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_preControl = SD_FALSE;
    
}

void manage_prectr_start(void)   
{

    m_prectr_thread();
}

void manage_prectr_stop(void)   
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_preControl = SD_FALSE;

}


//uart_get 的编写
/*void m_prectruart_thread(void)  //开启线程，UART信号的接收
{

        LPPressControlObjectDef pstPressControlObject = device_prectrl_object_get();
        if(SD_NULL != pstPressControlObject)
        {
            if ( pstPressControlObject->brun_prectrUart == SD_FALSE)
            {
                pstPressControlObject->brun_prectrUart = SD_TRUE;
                ut_thread_create(pstPressControlObject->Thead_prectrUart,"UARTRecv_PRECTR_THREAD",
                                            UT_THREAD_STACK_SIZE_LARGE,
                                            UT_THREAD_PRIORITY_DEFAULT,
                                            UT_THREAD_TICK_DEFAULT,
                                            bsp_uart_get,pstPressControlObject);  //uart发送数据的线程
            }
            rt_kprintf("  /m_prectr.c/ m_prectruart_thread run\n");   //添加输出打印

        }

}

void manage_prectruart_start(void)
{

    m_prectruart_thread();
}

void manage_prectruart_stop(void)
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_prectrUart = SD_FALSE;

}*/


