/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-0605      WangWei      the first version
 */

#include <rtthread.h>
#include "stdint.h"
#include "string.h"
#include "inc/b_prectrUART.h"
#include <rtdevice.h>

//判断是否读取一组数据结束的标志位
uint8_t uart_data[128] = {0};

// 接收数据回调函数
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem_4);

    return RT_EOK;
}


void bsp_uart_init(void)   //将压力控制器的uart设备放在其中，
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];  //创建一个长度为8的字符数组存储uart的名称
    char str[] = "PressureControl\r\n";
    rt_strncpy(uart_name, PreCtr_UART_NAME, RT_NAME_MAX);  

    //查找设备
    serial = rt_device_find(uart_name);   
    if (!serial)   
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    //初始化信号量
    rt_sem_init(&rx_sem_4, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;   //uart设备参数配置
    config.baud_rate = BAUD_RATE_38400; 
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);   //设备控制
    
    rt_device_open(serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);   //以读写和串口接收的方式打开设备

    //设置接收回调函数 
    rt_device_set_rx_indicate(serial, uart_input);

    //发送字符串 
   rt_device_write(serial, 0, str, (sizeof(str) - 1));   
    return ret;
}

void bsp_uart_get(PreCtrmqFrameDef* dmf)
{
    char ch;
    rt_size_t UartRec_bytes = 0;
    while (1)
    {
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&rx_sem_4, RT_WAITING_FOREVER);
        }

        
        //获取读取到的数据
        uart_data[UartRec_bytes++] = ch;
        if (UartRec_bytes == 3)
        {
            dmf->m_presorID = uart_data[1];  
            dmf->m_cmdtype = uart_data[2];   //对应功能 加压或减压

            // 清零操作
            UartRec_bytes = 0; 
        }
    }
}

        



