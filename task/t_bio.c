/*
 * @Author: wangwei wangwei@bitnei.com
 * @Date: 2023-04-28 10:33:37
 * @LastEditors: wangwei wangwei@bitnei.com
 * @LastEditTime: 2023-08-17 18:20:44
 * @FilePath: \compositesensor3\task\t_bio.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */

#include "inc/t_bio.h"
#include "bsp/inc/b_afe4300.h"

void task_thread_bio_recv(void *ptr)
{
    int cnt_recv_bio = 0;
    rt_kprintf("task_thread_bio_recv thread run\n");
    if(SD_NULL != ptr)
    {
        LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
        LPTaskObjectDef pstTaskObject = (LPTaskObjectDef)ptr;
        LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
        while (pstTaskObject->brun_bio)
        {
            //BioFrameDef dmf;
            SensorDataFrameDef dmf;  //2023.9.27
            dmf.m_bio_value = 0;
            bsp_afe4300_get(&dmf);      
            rt_kprintf("afe4300= %d\n",dmf.m_bio_value);
            ut_msg_send(pstMqueueObject->MMqueue_msg,2,0,emMqttMsgBioData,&dmf,sizeof(dmf));
           // rt_kprintf("[Task Module] ->task bio thread run\n");
            rt_thread_mdelay(1000);
            //cnt_recv_bio ++;
            //rt_kprintf("****cnt_recv_bio:%d\n", cnt_recv_bio);  //2023.9.27.打印发送bio计数
        }
        rt_kprintf("[Task Module] bio thread exit\n");
        ut_thread_exit(pstTaskObject->Taskthread_temp);
    }
}

 
 

void task_bio_init(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun_bio = SD_FALSE;
}

void task_bio_start(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    if(SD_NULL != pstTaskObject)
    {
        if (SD_FALSE == pstTaskObject->brun_bio)
        {
            pstTaskObject->brun_bio = SD_TRUE;
            ut_thread_create(pstTaskObject->Taskthread_bio,"TASK_BIO_THREAD",
                UT_THREAD_STACK_SIZE_LARGE,
                UT_THREAD_PRIORITY_DEFAULT,
                UT_THREAD_TICK_DEFAULT,
                task_thread_bio_recv,pstTaskObject);
        }
    }
}

void task_bio_stop(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun_bio = SD_FALSE;
}
