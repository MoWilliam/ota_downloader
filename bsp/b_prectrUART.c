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
#include "manage/inc/m_device.h"
#include <rtdevice.h>

struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; // UART 设备参数配置

uint8_t rx_rwflag = 0;
#define rx_rwflag_num 8
uint8_t rx_data[128] = {0};
extern rt_int32_t PreCtr_Flag;  //控制命令标志位
rt_int32_t PreCtr_WriteFlag;

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

void bsp_uart_get(PreCtrRecvFrameDef *dmf)
{
    uint8_t rx_data[8];
    int i =0;
    char hx_buf[256] = {0};
    char hx_data[3] = {0};

    rt_size_t read_count = rt_device_read(serial_4, 0, &rx_data, 8);
    if (read_count > 0)
    {
        PreCtr_WriteFlag = 1;  //表明有数据写入
        rt_kprintf("PreCtr_WriteFlag: %d\n", PreCtr_WriteFlag);
        dmf->msgID = (SdUInt16) ((rx_data[0] << 8) & 0xFFFF) | (rx_data[1] & 0xFFFF);  //msgid
        dmf->m_msgType = rx_data[2];
        dmf->m_deviceId = (SdUInt16) (rx_data[3] & 0xFFFF) | ((rx_data[4]<< 8) & 0xFFFF);  //deviceid

        dmf->m_deviceType = rx_data[5];
        dmf->m_cmdType = rx_data[6];
        dmf->m_pressureid = rx_data[7];
        dmf->m_stopFlag = 1;

        memset(hx_buf,0 ,sizeof(hx_buf));
        memset(hx_data,0 ,sizeof(hx_data));
        for (i = 0; i < 8; i++)
        {
            sprintf(hx_data,"%02x ", rx_data[i]);
            memcpy ( hx_buf + i*3,hx_data,3);
        }
        rt_kprintf("***********PreCtr_rev: %s\n", hx_buf);

    }
    else
    {
        // 如果未成功读取到数据，等待信号量释放
        rt_sem_take(&rx_sem_4, RT_WAITING_FOREVER);
    }
}

void bsp_uart_send(PreCtrFrameDef *dmf)
{
    int i =0;
    char hx_buf[256] = {0};
    char hx_data[3] = {0};

    char serial_data[8];
    serial_data[0] = (dmf->msgID >> 8) & 0xFF;
    serial_data[1] = dmf->msgID & 0xFF;
    serial_data[2] = dmf->m_msgType;
    serial_data[3] = 0x20; // 对齐位
    serial_data[4] = (dmf->m_deviceId >> 8) & 0xFF;
    serial_data[5] = dmf->m_deviceId & 0xFF;
    serial_data[6] = dmf->m_deviceType;
    serial_data[7] = dmf->m_cmdType;

    rt_device_write(serial_4, 0, serial_data, 8);


    memset(hx_buf,0 ,sizeof(hx_buf));
    memset(hx_data,0 ,sizeof(hx_data));
    for (i = 0; i < 8; i++)
    {
        sprintf(hx_data,"%02x ", serial_data[i]);
        memcpy ( hx_buf + i*3,hx_data,3);
    }
    rt_kprintf("***********PreCtr_Heart: %s\n", hx_buf);

    //rt_kprintf("PreCtr_Flag: %d\n", PreCtr_Flag);
}



