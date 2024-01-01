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


void print_heartbeat_info(PreCtrFrameDef *dmf);
static SdULong g_msgId_hearBeat;
#define LITTLE_TO_BIG_ENDIAN_16(x) ((uint16_t)((((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF)))  //交换高低字节
rt_int32_t PreCtr_Flag;
extern rt_int32_t PreCtr_WriteFlag;  //控制命令标志位



void thread_prectrheartbeat(void *ptr)   //建立一个发送的队列将心跳包发送给主控终端
{
    
    rt_kprintf("thread_prectrheartbeat thread run\n");
    if (SD_NULL != ptr)
    {
        
        LPPressControlObjectDef pstPressControlObject = (LPPressControlObjectDef)ptr;
        
        while (pstPressControlObject->brun_prectrheartBeat)
        {
            PreCtrFrameDef dmf;
            dmf.msgID = LITTLE_TO_BIG_ENDIAN_16(g_msgId_hearBeat++);
            dmf.m_msgType = 0x00;
            dmf.m_deviceType = 0x03;
            dmf.m_deviceId = get_STM32_uid(dmf.m_deviceId); //获取deviceid
            dmf.m_deviceId = LITTLE_TO_BIG_ENDIAN_16(dmf.m_deviceId);  //交换高低字节，依据小端模式
            dmf.m_cmdType = 0x00; 
            bsp_uart_send(&dmf);
            rt_thread_mdelay(1000*5);   //每隔10s发送一个心跳包，确保设备在线
        }
        rt_kprintf("[Thread Module] thread exit\n");
        ut_thread_exit(pstPressControlObject->Thead_prectrheartBeat);

    }
}


void commbyte_prectrheartBeat(void)     //创建心跳线程
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
        }
    }
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

/*

void thread_prectr_cmd(void *ptr)   //建立一个控制命令的函数
{
    
    rt_kprintf("thread_prectr_cmd thread run\n");
    if (SD_NULL != ptr)
    {
        
        LPPreCtrRecvFrameDef pstPreCtrRecvFrameDef = device_prectrlrecv_object_get();
        LPPreCtrFrameDef pstPreCtrFrameDef = device_prectrl_object_get();
        LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();  //消息队列
        LPPressControlObjectDef pstPressControlObject = (LPPressControlObjectDef)ptr;
        
        while (pstPressControlObject->brun_prectr_cmd)
        {
            if (pstPreCtrFrameDef)
            {
                PreCtrFrameDef dmf;
                //rt_kprintf("PreCtr_WriteFlag: %d\n", PreCtr_WriteFlag);
                //PreCtr_Flag = 0;
                if(PreCtr_WriteFlag==1 ){
                    dmf.msgID = LITTLE_TO_BIG_ENDIAN_16(pstPreCtrFrameDef->msgID );
                    //dmf.m_deviceId = pstPreCtrFrameDef->m_deviceId;
                    dmf.m_deviceId = LITTLE_TO_BIG_ENDIAN_16(pstPreCtrRecvFrameDef->m_deviceId);  //交换高低字节，依据小端模式
                    dmf.m_pressureid = pstPreCtrRecvFrameDef->m_pressureid;
                    dmf.m_deviceType = 0x03;
                    dmf.m_msgType = 0x00;
                    dmf.m_cmdType = pstPreCtrRecvFrameDef->m_cmdType;
                    rt_thread_mdelay(1000*0.3);
                    dmf.m_Ack = pstPreCtrFrameDef->m_Ack;


                    //dmf.m_cmdType = 0x05;
                    PreCtr_Flag = 1;
                    ut_mqueue_send(pstMqueueObject->MMqueue_prectrheartBeat, &dmf, sizeof(dmf));  //发送消息队列
                    dmf.m_Ack = 0x00;
                    PreCtr_WriteFlag = 0;
                }

            }

            rt_thread_mdelay(1000*1);   //每隔0.5s检测是否有命令下达
        }
        rt_kprintf("[Thread Module] thread exit\n");
        ut_thread_exit(pstPressControlObject->Thead_prectr_cmd);

    }
}
void commbyte_prectr_cmd(void)     //创建命令消息发送线程
{
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    if(SD_NULL != pstPressControlObject)
    {
        if ( pstPressControlObject->brun_prectr_cmd == SD_FALSE)   //检测线程是否存在，若不存在
        {
            pstPressControlObject->brun_prectr_cmd = SD_TRUE;
            ut_thread_create(pstPressControlObject->Thead_prectr_cmd,"PRECTR_CMD_THREAD",    //这边优先级的设置情况
                            UT_THREAD_STACK_SIZE_LARGE,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_DEFAULT,
                            thread_prectr_cmd,pstPressControlObject);
        }
    }
}

void manage_commbytecmd_init(void)
{
    g_msgId_hearBeat = 0;

}

void manage_commbytecmd_start(void) 
{  
    commbyte_prectr_cmd();
}


void manage_commbytecmd_stop(void)
{
    
    LPPressControlObjectDef pstPressControlObject = pressControl_ctrl_object_get();
    pstPressControlObject->brun_prectr_cmd = SD_FALSE;  // 设置标志位，停止心跳包线程
    rt_thread_delay(1000);  
    ut_thread_exit(pstPressControlObject->Thead_prectr_cmd);
    
}
*/
