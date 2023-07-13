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
cmdType： 命令类型定义 0：心跳命令；1：参数命令 2：加压；3：泄压  4：急停命令
PressureSensorId：控制的压力传感器设备
泄压命令调整  可以是过多长时间后自动泄压，也可以是通过主控终端接收到的压力数值调整泄压数值

*/

#include "inc/m_commbyte.h"
#include "bsp/inc/b_prectrgpio.h"
#include "bsp/inc/b_prectrUART.h"
#include "inc/m_device.h"
#include "ut/inc/ut_mq.h"

void print_heartbeat_info(const PreCtrFrameDef *message);

static SdULong g_msgId_hearBeat;

void mq_thread_prectrheartbeat(void *ptr)   //建立一个发送的队列将心跳包发送给主控终端
{
    rt_kprintf("mq_thread_prectrheartbeat thread run\n");
    if (SD_NULL != ptr)
    {
        LPPreCtrFrameDef pstPreCtrFrameDef = device_ctrl_object_get();
        LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get(); //消息队列
        LPPressControlObjectDef pstPressControlObject = (LPPressControlObjectDef)ptr;

        manage_prectrdevice_init();
        
        while (pstPressControlObject->brun_mqprectrheartBeat)
        {
            //if (pstPreCtrFrameDef && pstPreCtrFrameDef->m_deviceStatus == 1)  //2023.7.6修改
            if (pstPreCtrFrameDef)
            {
                PreCtrFrameDef message;
                message.msgID = g_msgId_hearBeat++;  
                strncpy(message.m_deviceid, "pressurecontrolsensor", DEVICE_LENGTH);   //设备id;
                message.m_deviceStatus = 1;  
                ut_mqueue_send(pstMqueueObject->MMqueue_preheartbeat, &message, sizeof(PreCtrFrameDef));
                print_heartbeat_info(&message);  //打印心跳包信息
                rt_kprintf("[MQ Module]-> prectrheartbeat thread run\n");
            }

            // 每隔10秒发送一个心跳包，确保设备在线
            rt_thread_mdelay(1000*5);   //每隔5s发送一个心跳包，确保设备在线
        }
        rt_kprintf("[MQ Module] thread exit\n");
        ut_thread_exit(pstMqueueObject->MMqueue_preheartbeat);
    }
}

void commbyte_mq_prectrheartBeat(void)     //开启线程
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    if(SD_NULL != pstPressControlObject)
    {
        if ( pstPressControlObject->brun_mqprectrheartBeat == SD_FALSE)   //检测线程是否存在，若不存在
        {
            pstPressControlObject->brun_mqprectrheartBeat = SD_TRUE;
            ut_thread_create(pstPressControlObject->MqThead_prectrheartBeat,"MQ_PRECTRHEARTBEAT_THREAD",    //这边优先级的设置情况
                            UT_THREAD_STACK_SIZE_LARGE,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_DEFAULT,
                            mq_thread_prectrheartbeat,pstPressControlObject);
        }
    }
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
                    rt_kprintf("UART SERVIER STATUS ONLINE\n");
                }
            }
            break;

        case emUartConnectOffline:
            {
                if ( pstPreCtrFrameDef)
                {
                    pstPreCtrFrameDef->m_deviceStatus = 0;
                    rt_kprintf("UART SERVIER STATUS OFFLINE\n");
                }
            }
            break;

        default:
            break;
    }
    return 0;
}

void print_heartbeat_info(const PreCtrFrameDef *message)
{
    rt_kprintf("Message ID: %d, Device ID: %s, Device Status: %d\n", message->msgID, message->m_deviceid, message->m_deviceStatus);

}

void manage_commbyte_init(void)
{
    g_msgId_hearBeat = 0;
    //g_msgId_control = 0;

}

void manage_commbyte_start(void) 
{  
    commbyte_mq_prectrheartBeat();
}


void manage_commbyte_stop(void)
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    if (pstPressControlObject != NULL)
    {
        pstPressControlObject->brun_mqprectrheartBeat = SD_FALSE;  // 设置标志位，停止心跳包线程
        ut_thread_exit(pstPressControlObject->MqThead_prectrheartBeat);
        g_msgId_hearBeat = 0;
    }
}


