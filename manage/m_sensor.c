/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-25     Administrator       the first version
 */
#include "inc/m_sensor.h"
#include "application.h"


void manage_thread_sensor_recv(void *ptr)
{
    rt_kprintf("manage_thread_sensor_recv thread run\n");
    if(SD_NULL != ptr)
    {
        LPManageObjectDef pstManageObject = (LPManageObjectDef)ptr;
        LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
        while (pstManageObject->brun_data)
        {
           DataFrameDef dmf;
           //SensorDataFrameDef dmf;  //2023.9.27
        //   ut_mqueue_recv(pstMqueueObject->MMqueue_sensor,&dmf,sizeof(dmf),RT_WAITING_FOREVER);
        //   rt_kprintf("[Manage Module] thread recv data %d ,%d ,%d \n",dmf.m_atemp,dmf.m_btemp,dmf.m_spo2);

            DataFrameDef mqttDmf;
            //SensorDataFrameDef mqttDmf;  //2023.9.21
            mqttDmf.m_atemp = 0;
            mqttDmf.m_btemp = 0;
        //    ut_msg_send(pstMqueueObject->MMqueue_msg,0,0,emMqttMsgBaseData,&mqttDmf,sizeof(mqttDmf));
            
            rt_kprintf("[Manage Module]->manage thread run\n");
            rt_thread_mdelay(500);
        }
        rt_kprintf("[Manage Module] thread exit\n");
        ut_thread_exit(pstManageObject->Managethread_data);
    }
}

void manage_sensor_init(void)
{
    LPManageObjectDef pstManageObject = manage_ctrl_object_get();
    pstManageObject->brun_data = SD_FALSE;
}

void manage_sensor_start(void)
{
    LPManageObjectDef pstManageObject = manage_ctrl_object_get();
    if(SD_NULL != pstManageObject)
    {
        /*
        if ( pstManageObject->brun_data == SD_FALSE)
        {
            pstManageObject->brun_data = SD_TRUE;
            ut_thread_create(pstManageObject->Managethread_data,"MANAGE_DATA_THREAD",
                            UT_THREAD_STACK_SIZE_DEFAULT,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_DEFAULT,
                            manage_thread_sensor_recv,pstManageObject);
        }
        */

    }
}

void manage_sensor_stop(void)
{
    LPManageObjectDef pstManageObject = manage_ctrl_object_get();
    pstManageObject->brun_data = SD_FALSE;
}
