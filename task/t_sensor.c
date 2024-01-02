/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-25     Administrator       the first version
 */
#include "inc/t_sensor.h"
#include "bsp/inc/b_jfh141.h"
#include "bsp/inc/b_max30205.h"

void task_thread_max30205_recv(void *ptr)
{
    //int cnt_recv_tmp = 0;
    SdInt16 last_m_atemp ;   //上次获得温度的整数有效值。
    SdInt16 last_m_btemp ;   //上次获得温度的小数有效值。
    rt_kprintf("task_thread_temp_recv thread run\n");
    if(SD_NULL != ptr)
    {
        LPTaskObjectDef pstTaskObject = (LPTaskObjectDef)ptr;
        LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
        while (pstTaskObject->brun)
        {
            //DataFrameDef dmf;
            SensorDataFrameDef dmf;  //2023.9.27
            dmf.m_atemp = 0;
            dmf.m_btemp = 0;
            bsp_max30205_get(&dmf);
            if(dmf.m_atemp>10 && dmf.m_atemp<50)  //去除温度显示过程中的极端异常值
            {
                rt_kprintf("max3025 %d %d",dmf.m_atemp,dmf.m_btemp);
                ut_msg_send(pstMqueueObject->MMqueue_msg,1,0,emMqttMsgBaseData,&dmf,sizeof(dmf));
            }
            rt_thread_mdelay(1000);
        }
        rt_kprintf("[Task Module] temp thread exit\n");
        ut_thread_exit(pstTaskObject->Taskthread_temp);
    }
}

void task_thread_jfh141_recv(void *ptr)
{
    //int cnt_recv_spo2 = 0;
    rt_kprintf("task_thread_temp_recv thread run\n");
    if(SD_NULL != ptr)
    {
        LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
        LPTaskObjectDef pstTaskObject = (LPTaskObjectDef)ptr;
        LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
        while (pstTaskObject->brun_spo2)
        {
            //Spo2FrameDef dmf;
            SensorDataFrameDef dmf;  //2023.9.27
            dmf.m_spo2 = 0;
            dmf.m_bk = 0;
            bsp_jfh141_get(&dmf); 
            rt_kprintf("jfh141 %d %d",dmf.m_spo2,dmf.m_bk);
            ut_msg_send(pstMqueueObject->MMqueue_msg,3,0,emMqttMsgSpo2Data,&dmf,sizeof(dmf));
            rt_thread_mdelay(1000);
        }
        //rt_kprintf("[Task Module] spo2 thread exit\n");
        ut_thread_exit(pstTaskObject->Taskthread_spo2);
    }
}

void task_sensor_init(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun = SD_FALSE;
    pstTaskObject->brun_bio = SD_FALSE;
}
 
void task_sensor_start(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    if(SD_NULL != pstTaskObject)
    {
        if (SD_FALSE == pstTaskObject->brun)
        {
            pstTaskObject->brun = SD_TRUE;
            ut_thread_create(pstTaskObject->Taskthread_temp,"TASK_TEMP_THREAD",
                UT_THREAD_STACK_SIZE_LARGE,
                UT_THREAD_PRIORITY_DEFAULT,
                UT_THREAD_TICK_DEFAULT,
                task_thread_max30205_recv,pstTaskObject);

            pstTaskObject->brun_spo2 = SD_TRUE;
            ut_thread_create(pstTaskObject->Taskthread_spo2,"TASK_SPO2_THREAD",
                UT_THREAD_STACK_SIZE_LARGE,
                UT_THREAD_PRIORITY_DEFAULT,
                UT_THREAD_TICK_DEFAULT,
                task_thread_jfh141_recv,pstTaskObject);
        }
    }
}

void task_sensor_stop(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun = SD_FALSE;
    pstTaskObject->brun_bio = SD_FALSE;
}
