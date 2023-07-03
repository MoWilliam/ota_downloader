/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-06-20 13:51:08
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-06-25 11:40:20
 * @FilePath: \pressurecontrolsensor\compositesensor\manage\m_prectr.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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
    char deviceid[DEVICE_ID_LENGTH];  //字符串类型
    rt_uint8_t cmdtype = 0;   //0，1类型
    rt_uint8_t flag = 0;      //用来存储急停按键的按键状态
    if(SD_NULL != ptr)
    {   

        LPAppObjectDef pstAppObject = (LPAppObjectDef)ptr;
        while(pstAppObject->brun )
        {
            uartRecvRet = rt_sem_take(&rx_sem_4, RT_WAITING_FOREVER);  // 获取USART4接收信号量，有返回的状态
            if (RT_EOK == uartRecvRet)
            {
                // 解析设备ID和命令类型
                sscanf((char *)g_prectrUart_RxBuf, "%s,%hhu", deviceid, cmdtype);

                // 根据设备ID选择设备，并根据命令类型确定设备的开关状态
                if (strcmp(deviceid, PressureSensor1) == 0)
                {
                    flag = get_key_pressed_flags(0);
                    if (cmdtype == PRESOR_ON && flag == 0)  //命令状态为press_on且急停按钮状态标志位为0，才能开启
                    {
                        bsp_PreCtr_GPIO_start(PressureSensor1);
                    }
                    else if (cmdtype == PRESOR_OFF)
                    {
                        bsp_PreCtr_GPIO_stop(PressureSensor1);
                    }
                }
                else if (strcmp(deviceid, PressureSensor2) == 0)
                {
                    flag = get_key_pressed_flags(1);
                    if (cmdtype == PRESOR_ON && flag == 0)
                    {
                        bsp_PreCtr_GPIO_start(PressureSensor2);
                    }
                    else if (cmdtype == PRESOR_OFF)
                    {
                        bsp_PreCtr_GPIO_stop(PressureSensor2);
                    }
                }
                else if (strcmp(deviceid, PressureSensor3) == 0)
                {
                    flag = get_key_pressed_flags(2);
                    if (cmdtype == PRESOR_ON && flag == 0)
                    {
                        bsp_PreCtr_GPIO_start(PressureSensor3);
                    }
                    else if (cmdtype == PRESOR_OFF)
                    {
                        bsp_PreCtr_GPIO_stop(PressureSensor3);
                    }
                }
                else if (strcmp(deviceid, PressureSensor4) == 0)
                {
                    flag = get_key_pressed_flags(3);
                    if (cmdtype == PRESOR_ON && flag == 0)
                    {
                        bsp_PreCtr_GPIO_start(PressureSensor4);
                    }
                    else if (cmdtype == PRESOR_OFF)
                    {
                        bsp_PreCtr_GPIO_stop(PressureSensor4);
                    }
                }
                else if (strcmp(deviceid, PressureSensor5) == 0)
                {
                    flag = get_key_pressed_flags(4);
                    if (cmdtype == PRESOR_ON && flag == 0)
                    {  
                        bsp_PreCtr_GPIO_start(PressureSensor5);
                    }
                    else if (cmdtype == PRESOR_OFF)
                    {
                        bsp_PreCtr_GPIO_stop(PressureSensor5);
                    }
                }

                memset(g_prectrUart_RxBuf, 0, UART_BUFFER_SIZE);  // 清空接收缓冲区
            }
            rt_thread_mdelay(100);
        }
    }
}

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
                                            bsp_uart_get,pstPressControlObject);;
            }

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


