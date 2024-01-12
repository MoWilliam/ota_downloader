/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-1017      WangWei      the first version
*/
#include "inc/b_adc.h"
#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>
#include <rtdbg.h>
#include <stdio.h>

/*
//电量读取
int Adc_Chargeread(void){
    rt_adc_device_t adc_dev;
    rt_uint32_t value;
    float vol;
    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if(adc_dev == RT_NULL){
        rt_kprintf("device : %s find failed!\n", ADC_DEV_NAME);
        return RT_ERROR;
    }
    rt_adc_enable(adc_dev, ADC_CHANNAL);
    while(1){
        value = 0;  // 为每组新读数重置 value
        for(int i=0; i<SAMPLE_NUM;i++){
            value += rt_adc_read(adc_dev, ADC_CHANNAL);
            rt_thread_mdelay(1000);
        }
        value = value / SAMPLE_NUM;
        vol = value * REG_VEL / 4095.0;
        printf("test adc value is %.2f\n", vol);
        rt_thread_mdelay(20);
    }
    return RT_EOK;
}*/
/*rt_uint32_t bsp_voltage_get(SensorDataFrameDef* dmf)   //获取电压数据
{
    rt_adc_device_t adc_dev;
    rt_uint32_t value, vol;
    rt_err_t ret = RT_EOK;

    // 查找设备
    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("adc run failed! can't find %s device!\n", ADC_DEV_NAME);
        return RT_ERROR;
    }

    // 使能设备
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);

    // 读取采样值
    value = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
    //rt_kprintf("the value is :%d \n", value);

    // 转换为对应电压值
    vol = value * REFER_VOLTAGE / CONVERT_BITS;
    rt_kprintf("the voltage is :%d.%02d \n", vol / 100, vol % 100);

    // 关闭通道
    ret = rt_adc_disable(adc_dev, ADC_DEV_CHANNEL);
    rt_kprintf("vol test:%s \n", ret);

    return vol;
}*/
