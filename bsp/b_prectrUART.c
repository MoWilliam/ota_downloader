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
#include <stdlib.h>
#include "inc/b_prectrUART.h"
#include <rtdevice.h>



#define BUFFER_SIZE 12
static char receive_buffer[BUFFER_SIZE];
static int receive_index = 0;

// 接收数据回调函数
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    // 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量
    rt_sem_release(&rx_sem_4);
    rt_kprintf("***444****\n"); // 打印测试
    return RT_EOK;
}

// 接收中断回调函数


/*
void bsp_uart_init(void)   //uart串口初始化
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];  //创建一个长度为8的字符数组存储uart的名称
    char str[] = "****333***\n";
    rt_strncpy(uart_name, PreCtr_UART_NAME, RT_NAME_MAX);  

    //查找设备
    serial_4 = rt_device_find(uart_name);
    if (!serial_4)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    //初始化信号量
    rt_sem_init(&rx_sem_4, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;   //uart设备参数配置
    config.baud_rate = BAUD_RATE_115200;
    rt_device_control(serial_4, RT_DEVICE_CTRL_CONFIG, &config);   //设备控制

    rt_device_open(serial_4, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);   //以读写和串口接收的方式打开设备
    
    //设置接收回调函数
    rt_device_set_rx_indicate(serial_4, uart_input);

    //发送字符串
   rt_device_write(serial_4, 0, str, (sizeof(str) - 1));
   //rt_kprintf("***333****\n"); // 打印测试

   
    return ret;
}*/




static rt_err_t uart_rx_callback(rt_device_t dev, rt_size_t size)
{
    rt_uint8_t ch;
    rt_size_t read_len = 0;

    while (read_len < size) {
        rt_device_read(dev, -1, &ch, 1);
        uart_rx_data.data[read_len] = ch;
        read_len++;
    }

    if (read_len == 5) {
        uart_rx_data.received = RT_TRUE;
    }

    return RT_EOK;
}

static void uart_rx_task(PreCtrFrameDef* dmf)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;   //uart设备参数配置

    rt_device_t uart_dev = rt_device_find(UART_DEV_NAME);
    rt_device_open(uart_dev, RT_DEVICE_FLAG_INT_RX);
    rt_device_control(uart_dev, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_set_rx_indicate(uart_dev, uart_rx_callback);

    while (1) {
        if (uart_rx_data.received == RT_TRUE) {
            // 数据匹配标志
            rt_bool_t data_matched = RT_TRUE;

            // 期望的数据，其中 * 表示通配符
            uint8_t expected_data[] = {0x00, 0x01, 0x10, 0x02, 0x01};

            // 数据格式以空格分隔
            char data_string[10]; // 假设最大长度为10
            snprintf(data_string, sizeof(data_string), "%c %c %c %c %c", 
                     uart_rx_data.data[0], uart_rx_data.data[1],
                     uart_rx_data.data[2], uart_rx_data.data[3],
                     uart_rx_data.data[4]);

            
            char* token = strtok(data_string, " ");
            int index = 0;
            
            while (token != RT_NULL && index < 5) {
                if (expected_data[index] != 0x2A &&  // 0x2A 表示通配符 *
                    (uint8_t)strtoul(token, RT_NULL, 16) != expected_data[index]) {
                    data_matched = RT_FALSE;
                    break;  // 一旦有不匹配的部分，跳出循环
                }
                token = strtok(RT_NULL, " "); // 获取下一个标记
                index++;
            }

            if (data_matched) {
                dmf->msgID = uart_rx_data.data[0];
                dmf->m_msgType = uart_rx_data.data[1];
                dmf->m_pressureid = uart_rx_data.data[2];
                dmf->m_deviceType = uart_rx_data.data[3];
                dmf->m_cmdType = uart_rx_data.data[4];
                rt_kprintf("received successfully\n");
            } else {
                rt_kprintf("received failed\n");
            }

            uart_rx_data.received = RT_FALSE;
        }

        rt_thread_mdelay(10);
    }
}


