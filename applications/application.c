/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */
#include "application.h"
#include "ut/inc/ut_mq.h"
#include "task/inc/t_sensor.h"
#include "task/inc/t_auth.h"
#include "task/inc/t_bio.h"
#include "task/inc/t_monitor.h"
#include "task/inc/t_power.h"
#include "manage/inc/m_comm.h"
#include "manage/inc/m_device.h"
#include "manage/inc/m_platform.h"
#include "manage/inc/m_sensor.h"
#include "bsp/inc/b_jfh141.h"
#include "bsp/inc/b_max30205.h"
#include "bsp/inc/b_esp8266.h"
#include "bsp/inc/b_afe4300.h"
#include "bsp/inc/b_prectrgpio.h"
#include "bsp/inc/b_prectrUART.h"

static AppObjectDef g_AppObject;
static TaskObjectDef g_TaskObject;
static ManageObjectDef g_ManageObject;
static MqueueObjectDef g_MqueueObject;
static MessageObjectDef g_MessageObject;
static PressControlObjectDef g_PressControlObject;

void app_thread_msg_recv(void *ptr);


LPAppObjectDef app_ctrl_object_get(void)
{
    return &g_AppObject;
}

LPTaskObjectDef task_ctrl_object_get(void)
{
    return &g_TaskObject;
}

LPManageObjectDef manage_ctrl_object_get(void)
{
    return &g_ManageObject;
}

LPMqueueObjectDef mq_ctrl_object_get(void)
{
    return &g_MqueueObject;
}

LPMessageObjectDef message_ctrl_object_get(void)
{
    return &g_MessageObject;
}

LPPressControlObjectDef pressControl_ctrl_object_get(void)
{
    return &g_PressControlObject;
}

void mq_ctrl_init(void)
{
    LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
    if (SD_NULL != pstMqueueObject)
    {
    #if COMPOSITE_CONTROL_FLAG
        pstMqueueObject->MMqueue_sensor = ut_mqueue_create("MQUEUE_SENSOR",
                        sizeof(DataFrameDef),
                        UT_MQUEUE_MINMSG_COUNT,RT_IPC_FLAG_FIFO);
                        
        pstMqueueObject->MMqueue_bio = ut_mqueue_create("MQUEUE_BIO",
                        sizeof(BioFrameDef),
                        UT_MQUEUE_MINMSG_COUNT,RT_IPC_FLAG_FIFO);

        pstMqueueObject->MMqueue_msg = ut_mqueue_create("MQUEUE_MSG",
                        UT_MQUEUE_MSGMAX_SIZE,
                        UT_MQUEUE_MAXMSG_COUNT,RT_IPC_FLAG_FIFO);
    #endif

    #if PRESS_CONTROL_FLAG
        pstMqueueObject->MMqueue_preheartbeat = ut_mqueue_create("MQUEUE_preUartHeartbeat",
                        UT_MQUEUE_MSGMAX_SIZE,
                        UT_MQUEUE_MAXMSG_COUNT,RT_IPC_FLAG_FIFO);
    #endif
    }
}

void mq_ctrl_unint(void)
{
    LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
    if (SD_NULL != pstMqueueObject)
    {
    #if COMPOSITE_CONTROL_FLAG
        ut_mqueue_delete(pstMqueueObject->MMqueue_msg);
        pstMqueueObject->MMqueue_msg = SD_NULL;
        ut_mqueue_delete(pstMqueueObject->MMqueue_bio);
        pstMqueueObject->MMqueue_bio = SD_NULL;
        ut_mqueue_delete(pstMqueueObject->MMqueue_sensor);
        pstMqueueObject->MMqueue_sensor = SD_NULL;
    #endif

    #if PRESS_CONTROL_FLAG
        ut_mqueue_delete(pstMqueueObject->MMqueue_preheartbeat);
        pstMqueueObject->MMqueue_preheartbeat = SD_NULL;
    #endif
    }
}

void message_ctrl_init(void)
{
    LPMessageObjectDef pstMessageObject = message_ctrl_object_get();
    if (SD_NULL != pstMessageObject)
    {
        pstMessageObject->MMessage_data =  ut_message_create("MANAGE_MESSAGE_DATA",
                        UT_MESSAGE_MAXMSG_SIZE,RT_IPC_FLAG_FIFO);
    }
}

void message_ctrl_uninit(void)
{
    LPMessageObjectDef pstMessageObject = message_ctrl_object_get();
    if (SD_NULL != pstMessageObject)
    {
        ut_message_delete(pstMessageObject->MMessage_data);
        pstMessageObject->MMessage_data = SD_NULL;
    }

}


/**
 * @brief  task module 
 * 
 * @return 
 */
void task_module_init(void)
{
    task_sensor_init();
    task_auth_init();
    task_bio_init();
    task_monitor_init();
    task_power_init();
}

void task_module_start(void)
{
    task_sensor_start();
    task_auth_start();
    task_bio_start();
    task_monitor_start();
    task_power_start();
}

void task_module_uninit(void)
{
    task_sensor_stop();
    task_auth_stop();
    task_bio_stop();
    task_monitor_stop();
    task_power_stop();
}

/**
 * @brief  manage module 
 * 
 * @return 
 */
void manage_module_init(void)
{
#if COMPOSITE_CONTROL_FLAG
    manage_comm_init();
    manage_sensor_init();
    manage_device_init();
    manage_platform_init();
#endif

#if PRESS_CONTROL_FLAG
    manage_commbyte_init();
    manage_prectr_init();
    manage_emerstop_init();
#endif
}

void manage_module_start(void)
{
#if COMPOSITE_CONTROL_FLAG
    manage_comm_start();
    manage_sensor_start();
    manage_device_start();
    manage_platform_start();
#endif

#if PRESS_CONTROL_FLAG
    manage_commbyte_start();  //启动心跳包线程
    manage_prectr_start();
    void manage_prectruart_start();
#endif
}

void manage_module_uninit(void)
{
#if COMPOSITE_CONTROL_FLAG
    manage_comm_stop();
    manage_sensor_stop();
    manage_device_stop();
    manage_platform_stop();
#endif
#if PRESS_CONTROL_FLAG
    manage_commbyte_stop();   //停止心跳包线程，标志位翻转
    manage_prectr_stop();
    manage_prectruart_stop(); 
#endif
}

/**
 * @brief  manage module 
 * 
 * @return 
 */
void bsp_module_init(void)
{
#if COMPOSITE_CONTROL_FLAG
    bsp_jfh141_init();
    bsp_max30205_init();
    bsp_esp8266_init();
    bsp_afe4300_init();
#endif
#if PRESS_CONTROL_FLAG
    bsp_PreCtr_GPIO_Init();
    bsp_uart_init();
#endif
}

void bsp_module_start(void)
{

}

void bsp_module_uninit(void)
{

}

void app_module_init(void)
{
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    pstAppObject->brun = SD_FALSE;
}
void app_module_start(void)
{
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    if(SD_NULL != pstAppObject)
    {
        if ( pstAppObject->brun == SD_FALSE)
        {
            pstAppObject->brun = SD_TRUE;
            ut_thread_create(pstAppObject->Appthread_msg,"APPLICATION_MSG_THREAD",
                            UT_THREAD_STACK_SIZE_LARGE,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_LARGE,
                            app_thread_msg_recv,pstAppObject);
        }

    }
}

void app_module_uninit(void)
{
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    pstAppObject->brun = SD_FALSE;
}

void app_thread_msg_recv(void *ptr)
{
    rt_kprintf("app_thread_msg_recv thread run\n");
    if(SD_NULL != ptr)
    {
        LPAppObjectDef pstAppObject = (LPAppObjectDef)ptr;
        LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
        while (pstAppObject->brun)
        {
            ut_msg_recv(pstMqueueObject->MMqueue_msg);
           // rt_kprintf("[App Module]-> msg thread run\n");
            rt_thread_mdelay(100);
        }
        rt_kprintf("[App Module] thread exit\n");
        ut_thread_exit(pstAppObject->Appthread_msg);
    }
}

/**
 * @brief  application start
 * 
 * @return 
 */
void appStart(void)
{
    // mqueue 消息队列初始化
    mq_ctrl_init();
    // message 短消息队列初始化
    message_ctrl_init();

    app_module_init();
    app_module_start();

    // manage 模块初始化
    manage_module_init();
    manage_module_start();

    // BSP 驱动初始化
    rt_hw_us_delay(1000*100);
    bsp_module_init();
    rt_hw_us_delay(1000*100);
    bsp_module_start();

    // task 模块初始化
    task_module_init();
    // task 模块启动
    task_module_start();

}

/**
 * @brief  application stop
 * 
 * @return 
 */
void appStop(void)
{
    task_module_uninit();
    manage_module_uninit();
    bsp_module_uninit();
    app_module_uninit();

    mq_ctrl_unint();
    message_ctrl_uninit();

}


SdInt app_msg_handle(const UTMsgDef * pMsg, const void * pContent)
{
    //rt_kprintf("[App Msg Handle] usMsgId %d\n",pMsg->usMsgID);
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
	switch(pMsg->usMsgID)
	{	
		case emMqttMsgBaseData:
		case emMqttMsgSpo2Data:
		case emMqttMsgBioData:
            if ( pstDeviceObject)
            {
                if ( pstDeviceObject->m_mqttStatus == 1)
                {
                 if (pstDeviceObject->m_deviceStatus == 1)
                    {
                        comm_mqtt_msg(pMsg,pContent);
                    }
                }
            }
			break;
    }
	return 0;
}