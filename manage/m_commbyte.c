/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-06-02     WangW       the first version
 */

/*
该模块主要是 uart的字节流的数据传输和接收,并对接收到的相关命令进行操作


msgId：消息ID
msgType：  上传：1    下传:2
deviceId：设备ID
deviceType：采集终端设备类型： 1、综合采集器   2、压力传感器  3、气动单元
cmdType： 命令类型定义 0：心跳命令；1：参数命令 2：加压；3：泄压
PressureSensorId：控制的压力传感器设备 0x10 0x11 0x12 0x13 0x14

*/

#include "inc/m_commbyte.h"
#include "bsp/inc/b_prectrgpio.h"
#include "bsp/inc/b_prectrUART.h"
#include "inc/m_device.h"
#include "ut/inc/ut_mq.h"

void print_heartbeat_info(const PreCtrFrameDef *message);

static SdULong g_msgId_hearBeat;

void thread_prectrheartbeat(void *ptr)   //建立一个发送的队列将心跳包发送给主控终端
{
    rt_kprintf("thread_prectrheartbeat thread run\n");
    if (SD_NULL != ptr)
    {

        LPPreCtrFrameDef pstPreCtrFrameDef = device_ctrl_object_get();
        //LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get(); //消息队列
        LPPressControlObjectDef pstPressControlObject = (LPPressControlObjectDef)ptr;
        //manage_prectrdevice_init();  //初始化数组
        
        while (pstPressControlObject->brun_prectrheartBeat)
        {
            if (pstPreCtrFrameDef)
            {

                PreCtrFrameDef dmf;

                //bsp_uart_get(&dmf);
                //rt_kprintf("***111***\n");
                dmf.msgID = g_msgId_hearBeat++; 
                char STM32_DEVICEID[DEVICE_LENGTH];
                get_STM32_uid(STM32_DEVICEID);
                strcpy(dmf.m_deviceId, STM32_DEVICEID);   //将芯片uid号赋值过去
                print_heartbeat_info(&dmf);  //打印心跳包信息

                
                rt_kprintf("[Thread Module]-> prectrheartbeat thread run\n");
            }

            // 每隔10秒发送一个心跳包，确保设备在线
            rt_thread_mdelay(1000);   //每隔10s发送一个心跳包，确保设备在线
        }
        rt_kprintf("[Thread Module] thread exit\n");
        ut_thread_exit(pstPressControlObject->Thead_prectrheartBeat);

    }
}

void commbyte_prectrheartBeat(void)     //创建线程
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    if(SD_NULL != pstPressControlObject)
    {
        if ( pstPressControlObject->brun_prectrheartBeat == SD_FALSE)   //检测线程是否存在，若不存在
        {
            pstPressControlObject->brun_prectrheartBeat = SD_TRUE;
            ut_thread_create(pstPressControlObject->Thead_prectrheartBeat,"PRECTRHEARTBEAT_THREAD",    //这边优先级的设置情况
                            UT_THREAD_STACK_SIZE_LARGE,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_DEFAULT,
                            thread_prectrheartbeat,pstPressControlObject);
            rt_kprintf("***222****\n");
        }
    }
}

//心跳包信息的打印
void print_heartbeat_info(const PreCtrFrameDef *dmf)
{
    rt_kprintf("***666***\n");
    rt_kprintf("Message ID: %d, Message Type: %d, Device Type: %d, Device Id: %d, Cmd Type: %d\n",
                dmf->msgID, dmf->m_msgType, dmf->m_deviceType, dmf->m_deviceId, dmf->m_cmdType);

}

void manage_commbyte_init(void)
{
    g_msgId_hearBeat = 0;

}

void manage_commbyte_start(void) 
{  
    commbyte_prectrheartBeat();
}


void manage_commbyte_stop(void)
{
    
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_prectrheartBeat = SD_FALSE;  // 设置标志位，停止心跳包线程
    rt_thread_delay(1000);  
    ut_thread_exit(pstPressControlObject->Thead_prectrheartBeat);
    
}


//连接状态的状态标明
SdInt commbyte_status(int connectStatus)
{
    LPPreCtrFrameDef pstPreCtrFrameDef = device_ctrl_object_get();
    switch (connectStatus) {
        case emUartConnectOk:
            {
                if ( pstPreCtrFrameDef)
                {
                    pstPreCtrFrameDef->m_deviceStatus = 2;
                    rt_kprintf("UART SERVIER STATUS CONNECTED\n");
                }
            }
            break;
        case emUartConnectOnline:
            {
                if ( pstPreCtrFrameDef)
                {
                    pstPreCtrFrameDef->m_deviceStatus = 1;
                    rt_kprintf("UART SERVIER STATUS Start\n");
                }
            }
            break;

        case emUartConnectOffline:
            {
                if ( pstPreCtrFrameDef)
                {
                    pstPreCtrFrameDef->m_deviceStatus = 0;
                    rt_kprintf("UART SERVIER STATUS Stop\n");
                }
            }
            break;

        default:
            break;
    }
    return 0;
}
