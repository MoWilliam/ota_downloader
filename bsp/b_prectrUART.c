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

//static struct uart_data uart_rx_data = {0};

//rt_mq_t uart4_rx_mq;

struct mq_msg
{
    rt_device_t dev;
    rt_size_t size;
};

static rt_err_t uart4_rx_callback(rt_device_t dev, rt_size_t size)
{
    LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
    struct mq_msg msg;
    msg.dev = dev;
    msg.size = size;
    if (SD_NULL != pstMqueueObject)
    {
        ut_mqueue_send(pstMqueueObject->MMqueue_prectrUART, &msg, sizeof(msg));
    }
    return RT_EOK;
}

void bsp_uart_init(void)
{
    char send_str[] = "Uart4 is ok/r/n";
    rt_size_t send_size = 0;

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; // UART 设备参数配置

    serial_4 = rt_device_find(UART_DEV_NAME);
    if (!serial_4)
    {
        rt_kprintf("find %s failed!\n", UART_DEV_NAME);
        return RT_ERROR;
    }
    rt_sem_init(&rx_sem_4, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    
    //config.baud_rate = 38400;
    //config.bufsz = 128; //缓冲区大小
    rt_device_control(serial_4, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_open(serial_4, RT_DEVICE_FLAG_DMA_RX);
    rt_device_set_rx_indicate(serial_4, uart4_rx_callback);
    send_size = rt_device_write(serial_4,0,send_str,sizeof(send_str));
    rt_kprintf("the length of send string : %d\n", send_size);
    
}

void bsp_uart_get(PreCtrFrameDef *dmf)
{
    LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();
    struct mq_msg msg;
    char rx_buf[128];
    rt_size_t rx_len;

    while(1)
    {
        //将队列中的内容清零

        rt_memset(&msg, 0, sizeof(msg));
        rt_memset(&rx_buf, 0, 128);
        if (SD_NULL != pstMqueueObject)
        {
            //rt_kprintf("***555****\n");
            //判断是否有接收到消息
            if(ut_mqueue_recv(pstMqueueObject->MMqueue_prectrUART, &msg, sizeof(msg), RT_WAITING_FOREVER ) == RT_EOK)
            {
                //rt_kprintf("***666****\n");

                rx_len = rt_device_read(msg.dev, 0, &rx_buf, msg.size);
                //memcpy(&dmf, rx_buf, sizeof(PreCtrFrameDef));
                //rt_kprintf("rx_len = %d content: %s\n", rx_len, rx_buf);
                if (rx_len == sizeof(PreCtrFrameDef))
                {
                    dmf->msgID = rx_buf[0];
                    dmf->m_msgType = rx_buf[1];
                    dmf->m_pressureid = rx_buf[2];
                    dmf->m_deviceType = rx_buf[3];
                    dmf->m_cmdType = rx_buf[4];   
                    rt_kprintf("Received data: msgID=%02X, m_msgType=%02X, m_pressureid=%02X, m_deviceType=%02X, m_cmdType=%02X\n",
                                dmf->msgID, dmf->m_msgType, dmf->m_pressureid, dmf->m_deviceType, dmf->m_cmdType);
                }else
                {
                    rt_kprintf("Received data has incorrect length.\n");
                }
                
            }
        }
        rt_thread_mdelay(1000);
        
    }
}

    



