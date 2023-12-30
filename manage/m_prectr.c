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
该模块主要是  读取主控终端发送到的数据，并进行操作命令（app上 设备id、加压和泄压操作）

**/

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "bsp/inc/b_prectrgpio.h"
#include "bsp/inc/b_prectrUART.h"
#include "inc/m_commbyte.h"
#include "inc/m_prectr.h"
#include "inc/m_emerstop.h"

extern rt_int32_t PreCtr_Flag;  //控制命令标志位

void prectr_recv_thread_entry(void *ptr)    //线程任务
{
    rt_err_t uartRecvRet = RT_EOK;
    rt_uint8_t flag = 0;      //用来存储急停按键的按键状态

    if(SD_NULL != ptr)
    {

        LPPressControlObjectDef pstPressControlObject = (LPPressControlObjectDef)ptr;
        LPPreCtrFrameDef pstPreCtrFrameDef = device_prectrl_object_get();
        LPPreCtrRecvFrameDef pstPreCtrRecvFrameDef = device_prectrlrecv_object_get();

        while(pstPressControlObject->brun_preControl )
        {
            // 根据设备ID选择设备，并根据命令类型确定设备的开关状态


            if(get_key_pressed_flags() == emUARTCmdStopPress_FlagOn)
            {
                
                PreCtr_Flag = 1;

                bsp_PreCtr_GPIO_stopall();
                
                set_key_pressed_flags(0);
                pstPreCtrRecvFrameDef->m_stopFlag = 0;

                rt_kprintf("  /m_prectr.c/ emerstop PressureSensor\n");   //添加输出打印
            }
            else if ( pstPreCtrRecvFrameDef->m_stopFlag == 1)
            {
                if (strcmp(pstPreCtrRecvFrameDef->m_pressureid, PressureSensor1) == 0)
                    {
                        if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStartPress)  //命令状态为press_on且急停按钮状态标志位为0，才能开启
                        {
                            bsp_PreCtr_GPIO_start(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ start PressureSensor1\n");   //添加输出打印
                        }
                        else if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStopPress )
                        {
                            bsp_PreCtr_GPIO_stop(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ stop PressureSensor1\n");   //添加输出打印
                        }
                    }else if (strcmp(pstPreCtrRecvFrameDef->m_pressureid, PressureSensor2) == 0)
                    {
                        if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStartPress)  //命令状态为press_on且急停按钮状态标志位为0，才能开启
                        {
                            bsp_PreCtr_GPIO_start(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ start PressureSensor2\n");   //添加输出打印
                        }
                        else if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStopPress )
                        {
                            bsp_PreCtr_GPIO_stop(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ stop PressureSensor2\n");   //添加输出打印
                        }
                    }else if (strcmp(pstPreCtrRecvFrameDef->m_pressureid, PressureSensor3) == 0)
                    {
                        if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStartPress)  //命令状态为press_on且急停按钮状态标志位为0，才能开启
                        {
                            bsp_PreCtr_GPIO_start(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ start PressureSensor3\n");   //添加输出打印
                        }
                        else if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStopPress )
                        {
                            bsp_PreCtr_GPIO_stop(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ stop PressureSensor3\n");   //添加输出打印
                        }
                    }else if (strcmp(pstPreCtrRecvFrameDef->m_pressureid, PressureSensor4) == 0)
                    {
                        if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStartPress)  //命令状态为press_on且急停按钮状态标志位为0，才能开启
                        {
                            bsp_PreCtr_GPIO_start(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ start PressureSensor4\n");   //添加输出打印
                        }
                        else if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStopPress )
                        {
                            bsp_PreCtr_GPIO_stop(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ stop PressureSensor4\n");   //添加输出打印
                        }
                    }else if (strcmp(pstPreCtrRecvFrameDef->m_pressureid, PressureSensor5) == 0)
                    {
                        if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStartPress)  //命令状态为press_on且急停按钮状态标志位为0，才能开启
                        {
                            bsp_PreCtr_GPIO_start(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ start PressureSensor5\n");   //添加输出打印
                        }
                        else if (pstPreCtrRecvFrameDef->m_cmdType == emUartCmdStopPress )
                        {
                            bsp_PreCtr_GPIO_stop(pstPreCtrRecvFrameDef->m_pressureid);
                            rt_kprintf("  /m_prectr.c/ stop PressureSensor5\n");   //添加输出打印
                        }
                    }

            memset(g_prectrUart_RxBuf, 0, UART_BUFFER_SIZE);  // 清空接收缓冲区
            rt_thread_mdelay(1000);
        }

    }
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