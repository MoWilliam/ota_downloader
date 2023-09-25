/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
/*
#include "inc/t_power.h"
#include "bsp/inc/b_jfh141.h"
#include "bsp/inc/b_afe4300.h"
#include <drivers/pm.h>
#include <rtthread.h>
#include "rtdevice.h"
#include "drivers/board.h"

static rt_tick_t period_start_time = 0; //计时器变量
static SdUInt16 prev_bio_value = 0;
static SdUInt16 prev_spo2_value = 0;
static rt_bool_t sleep_flag = RT_PM_EXIT_SLEEP;  //PWR是否开启状态标志位，enter_sleep:0 ; exit_sleep:1。
#define GPIO_Sleep_PIN    (GET_PIN(B, 1))     


int check_sleep_flag(void);
int enter_sleep(void);




//中断回调函数
void GPIO_Sleep_irq_callback(void *args)
{
    LOG_D("GPIO irq callback");
}
//设置回调函数
static void pm_callback(rt_uint8_t event, uint8_t mode, void *data)
{   
    check_sleep_flag();
    event = sleep_flag;
    //rt_kprintf("*********************1111111111111111\n");
    if(event == RT_PM_ENTER_SLEEP && mode == PM_SLEEP_MODE_DEEP)
    {
        enter_sleep(); //进入light_sleep模式

    }
    else if (event == RT_PM_EXIT_SLEEP)
    {
        
        rt_pm_run_enter(PM_RUN_MODE_HIGH_SPEED);

        rt_pm_release(PM_SLEEP_MODE_DEEP);  // 释放 Light_sleep 模式
        rt_pm_request(PM_SLEEP_MODE_NONE);  // 请求工作模式
        
    } 
}


//中断函数
void GPIO_Sleep_irq_init(void)
{   

    rt_pin_mode(GPIO_Sleep_PIN, PIN_MODE_OUTPUT);    // 配置为输出模式
    rt_pin_write(GPIO_Sleep_PIN,PIN_HIGH);
    check_sleep_flag();
    if( sleep_flag = RT_PM_ENTER_SLEEP){
        rt_pin_write(GPIO_Sleep_PIN,PIN_LOW);
        rt_pin_attach_irq(GPIO_Sleep_PIN, PIN_IRQ_MODE_LOW_LEVEL, GPIO_Sleep_irq_callback, RT_NULL);   // 下降沿触发
        rt_pin_irq_enable(GPIO_Sleep_PIN, PIN_IRQ_ENABLE);  // 使能中断
    }
}

int check_sleep_flag(void)
{
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
    LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
    BioFrameDef dmf_bio;
    dmf_bio.m_bio_value = 0;
    bsp_afe4300_get(&dmf_bio);
    Spo2FrameDef dmf_spo2;
    dmf_spo2.m_spo2 = 0;
    bsp_jfh141_get(&dmf_spo2);

    rt_tick_t current_time = rt_tick_get();

    //判断是否开启新周期,以60s为周期，周期内数值没有变化则置状态位为1，对设备开启低功耗模式，如果有变化则维持不变。
    if( period_start_time == 0 || current_time - period_start_time >= 60*RT_TICK_PER_SECOND)
    {
        period_start_time = current_time;
        sleep_flag = RT_PM_ENTER_SLEEP;
    }else if(dmf_bio.m_bio_value != prev_bio_value ||  dmf_spo2.m_spo2 != 0){  
        period_start_time = current_time;
        sleep_flag = RT_PM_EXIT_SLEEP;
        prev_bio_value = dmf_bio.m_bio_value;
        prev_spo2_value = dmf_spo2.m_spo2;
    }
    return sleep_flag;
}

int enter_sleep(void)
{
    rt_pm_request(PM_SLEEP_MODE_DEEP);  // 请求 light_sleep模式
    //rt_pm_dump_status();  // 打印 PM 组件状态
    //rt_kprintf("*********************333333333333333333333333333\n");
    rt_pm_release(PM_SLEEP_MODE_NONE);  // 释放正常工作模式，释放后才能进入 stop 模式
    //rt_pm_dump_status();  // 打印 PM 组件状态

    return 0;
}

int task_thread_PM_manage(void *ptr)
{
    if (SD_NULL != ptr)
    {
        
        LPTaskObjectDef pstTaskObject = (LPTaskObjectDef)ptr;
        clock_information();  // 打印系统时钟信息，打印一下看看 PM 组件是不是工作在 Normal 模式
        
        
        while(1){

            rt_pm_notify_set(pm_callback, 0);    // 设置回调函数

            GPIO_Sleep_irq_init();
            rt_thread_mdelay(1000);
        }

    }
    return RT_EOK;
}

void task_power_init(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun_sensor = SD_FALSE;
}

void task_power_start(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    if(SD_NULL != pstTaskObject)
    {
        if (SD_FALSE == pstTaskObject->brun_sensor)
        {
            pstTaskObject->brun_sensor = SD_TRUE;
            ut_thread_create(pstTaskObject->Taskthread_power,"TASK_POWER_THREAD",
                UT_THREAD_STACK_SIZE_LARGE,
                UT_THREAD_PRIORITY_DEFAULT,
                UT_THREAD_TICK_DEFAULT,
                task_thread_PM_manage,pstTaskObject);
        }

    }
}


void task_power_stop(void)
{
    LPTaskObjectDef pstTaskObject = task_ctrl_object_get();
    pstTaskObject->brun_sensor = SD_FALSE;
}
*/
