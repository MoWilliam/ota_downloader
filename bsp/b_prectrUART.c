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


static struct uart_data uart_rx_data = {0};
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; // UART 设备参数配置

static rt_err_t uart4_rx_callback(rt_device_t dev, rt_size_t size)
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

void bsp_uart_init(void)
{

    uint8_t tx_data[] = {0x00, 0x00, 0x00, 0x00, 0x00};

    rt_size_t send_size = 0;

    serial_4 = rt_device_find(UART_DEV_NAME);
    if (!serial_4)
    {
        rt_kprintf("find %s failed!\n", UART_DEV_NAME);
        return RT_ERROR;
    }
    rt_sem_init(&rx_sem_4, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    rt_device_open(serial_4, RT_DEVICE_FLAG_INT_RX);
    config.bufsz = 128;
    rt_device_control(serial_4, RT_DEVICE_CTRL_CONFIG, &config);

    //中断回调函数
    rt_device_set_rx_indicate(serial_4, uart4_rx_callback);

    send_size = rt_device_write(serial_4, 0, tx_data, sizeof(tx_data));

    rt_kprintf("the length of send string : %d\n", send_size);


}

void bsp_uart_get(PreCtrFrameDef *dmf)
{
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

        char *token = strtok(data_string, " ");
        int index = 0;

        while (token != RT_NULL && index < 5) {
            if (expected_data[index] != 0x2A && // 0x2A 表示通配符 *
                (uint8_t)strtoul(token, RT_NULL, 16) != expected_data[index]) {
                data_matched = RT_FALSE;
                break; // 一旦有不匹配的部分，跳出循环
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
        } else {
            rt_kprintf("data receive failed!\n");
            return RT_ERROR;// 执行数据不匹配时的错误处理
        }

        uart_rx_data.received = RT_FALSE;
    }
}


