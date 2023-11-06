/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-0605      WangWei      the first version
 */
/*uart的初始化  串口读写功能的*/
#include <rtthread.h>
#include "stdint.h"
#include "string.h"
#include <stdlib.h>
#include "inc/b_prectrUART.h"
#include <rtdevice.h>

struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; // UART 设备参数配置

uint8_t rx_rwflag = 0;
#define rx_rwflag_num 7
uint8_t rx_data[128] = {0};


static rt_err_t uart4_rx_callback(rt_device_t dev, rt_size_t size)
{

    rt_sem_release(&rx_sem_4);  //释放信号量
    return RT_EOK;
}


void bsp_uart_init(void)
{
    rt_err_t ret = 0;
    //char send_str[] = "Uart4 is ok/r/n";

    rt_size_t send_size = 0;

    serial_4 = rt_device_find(UART_DEV_NAME);
    if (!serial_4)
    {
        rt_kprintf("find %s failed!\n", UART_DEV_NAME);
        return RT_ERROR;
    }
    rt_sem_init(&rx_sem_4, "rx_sem", 0, RT_IPC_FLAG_FIFO);   //初始化信号量
    ret = rt_device_open(serial_4, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);  //以读写和中断接收的方式
    if(ret < 0){
        rt_kprintf("open %s failed!\n", UART_DEV_NAME);
                return RT_ERROR;
    }
    //config.baud_rate = 38400;
    config.bufsz = 128;
    rt_device_control(serial_4, RT_DEVICE_CTRL_CONFIG, &config);

    //中断回调函数
    rt_device_set_rx_indicate(serial_4, uart4_rx_callback);
    //send_size = rt_device_write(serial_4,0,send_str,sizeof(send_str));

    //rt_kprintf("the length of send string : %d\n", send_size);


}



void bsp_uart_get(PreCtrFrameDef *dmf)
{
    char ch;
    
    LPPreCtrFrameDef pstPreCtrFrameDef = device_prectrl_object_get();
    //PreCtrFrameDef dmf;
    while (1) {
        // 尝试从串口读取一个字节的数据
        rt_size_t read_count = rt_device_read(serial_4, 0, &ch, 1);

        if (read_count > 0) {
            
            rx_data[rx_rwflag++] = ch;
            if (rx_rwflag >= rx_rwflag_num)
            {
                if(rx_data[0] == 0x00 && rx_data[rx_rwflag_num-2] == 0x01 && rx_data[rx_rwflag_num-1] == 0x01)  //设立校验帧
                {

                    pstPreCtrFrameDef->m_msgType = rx_data[1];
                    pstPreCtrFrameDef->m_pressureid = rx_data[2];
                    pstPreCtrFrameDef->m_deviceType = rx_data[3];
                    pstPreCtrFrameDef->m_cmdType = rx_data[4];




                }else{
                    rt_kprintf("UART4 Recv failed!!!\n");
                }
                rx_rwflag = 0;

            }

        } else {
            // 如果未成功读取到数据，等待信号量释放
            rt_sem_take(&rx_sem_4, RT_WAITING_FOREVER);
        }
    }

    //rt_thread_mdelay(50);
}

void bsp_uart_send(PreCtrFrameDef *dmf){

    //rt_size_t i;
    LPPreCtrFrameDef pstPreCtrFrameDef = device_prectrl_object_get();
    LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();  //消息队列
    while(1){
        PreCtrFrameDef dmf;
        rt_memset(&dmf, 0, sizeof(dmf));

        //判断队列是否接收到消息
        if(ut_mqueue_recv(pstMqueueObject->MMqueue_prectrheartBeat, &dmf, sizeof(dmf),RT_WAITING_FOREVER) == RT_EOK){
            rt_device_write(serial_4, 0, &dmf, 14);
            //rt_kprintf("***divice Id: %s\n",dmf.m_deviceId);
        }
        rt_thread_mdelay(1000*2);
    }

}


