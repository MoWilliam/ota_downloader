/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-24     Administrator       the first version
 */
#include "ut/inc/ut_uart.h"
#include <rtthread.h>
#include <rtdevice.h>

#define AT_SAMPLE_UART_NAME       "uart2"      /* 串口设备名称 */

/* 用于接收消息的信号量 */
static struct rt_semaphore at_rx_sem;
static rt_device_t at_serial;

/* 接收数据回调函数 */
static rt_err_t at_uart_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&at_rx_sem);

    return RT_EOK;
}

static void at_serial_thread_entry(void *parameter)
{
    char ch;

    while (1)
    {
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(at_serial, -1, &ch, 1) != 1)
        {
            rt_kprintf("uart2-> %c \n", ch);
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&at_rx_sem, RT_WAITING_FOREVER);

        }
    }
}

int at_uart_init(void)
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    char str[] = "AT+CWMODE=2\r\n";

    rt_strncpy(uart_name, AT_SAMPLE_UART_NAME, RT_NAME_MAX);

    /* 查找串口设备 */
    at_serial = rt_device_find(uart_name);
    if (!at_serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    /* 初始化信号量 */
    rt_sem_init(&at_rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 修改串口配置 */
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = BAUD_RATE_115200;
    rt_device_control(at_serial, RT_DEVICE_CTRL_CONFIG, &config);
    /* 以中断接收及轮询发送方式打开串口设备 */
    rt_device_open(at_serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(at_serial, at_uart_input);
    /* 发送字符串 */
    rt_device_write(at_serial, 0, str, (sizeof(str) - 1));

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("serial", at_serial_thread_entry, RT_NULL, 1024, 25, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }


    return ret;
}

void at_uart_uninit(void)
{
    /* 关闭串口 */
    rt_device_close(at_serial);
}
