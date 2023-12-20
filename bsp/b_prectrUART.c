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
                PreCtr_WriteFlag = 1;  //表明有数据写入
                rt_kprintf("PreCtr_WriteFlag: %d\n", PreCtr_WriteFlag);
                pstPreCtrFrameDef->msgID = (SdUInt16) ((rx_data[0] << 8) & 0xFFFF) | (rx_data[1] & 0xFFFF);  //msgid
                pstPreCtrFrameDef->m_msgType = rx_data[2];
                pstPreCtrFrameDef->m_deviceId = (SdUInt16) ((rx_data[3] << 8) & 0xFFFF) | (rx_data[4] & 0xFFFF);  //deviceid

                pstPreCtrFrameDef->m_deviceType = rx_data[5];
                pstPreCtrFrameDef->m_cmdType = rx_data[6];
                pstPreCtrFrameDef->m_pressureid = rx_data[7];

                rx_rwflag = 0;
                #if DeBug
                rt_kprintf("***recive_pressureid: %d\n",pstPreCtrFrameDef->m_pressureid);
                rt_kprintf("***recive_cmdtype: %d\n",pstPreCtrFrameDef->m_cmdType);

                #endif

            }

        } else {
            // 如果未成功读取到数据，等待信号量释放
            rt_sem_take(&rx_sem_4, RT_WAITING_FOREVER);
        }
    }

    rt_thread_mdelay(50);
}



/*void bsp_uart_send(PreCtrFrameDef *dmf){


    LPPreCtrFrameDef pstPreCtrFrameDef = device_prectrl_object_get();
    LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get();  //消息队列
    while(1){
        PreCtrFrameDef dmf;
        rt_memset(&dmf, 0, sizeof(dmf));

        //判断队列是否接收到消息
        if(ut_mqueue_recv(pstMqueueObject->MMqueue_prectrheartBeat, &dmf, sizeof(dmf),RT_WAITING_NO) == RT_EOK){
            if(PreCtr_Flag == 0){   //心跳信息
                rt_device_write(serial_4, 0, &dmf, 8);
                rt_kprintf("PreCtr_Flag: %d\n", PreCtr_Flag);
            }else if (PreCtr_Flag == 1){   //控制命令返回信息（数据上传）
                rt_device_write(serial_4, 0, &dmf, sizeof(dmf));

                rt_kprintf("PreCtr_Flag: %d\n", PreCtr_Flag);
            }else{
                rt_kprintf("UART Send is failed\n");
            }



        }
        rt_thread_mdelay(50);
    }

}*/

void bsp_uart_send(){
    LPPreCtrFrameDef pstPreCtrFrameDef = device_prectrl_object_get();
    LPMqueueObjectDef pstMqueueObject = mq_ctrl_object_get(); //消息队列
    PreCtrFrameDef dmf;
    rt_memset(&dmf, 0, sizeof(dmf));
    //判断队列是否接收到消息
    if(ut_mqueue_recv(pstMqueueObject->MMqueue_prectrheartBeat, &dmf, sizeof(dmf),RT_WAITING_NO) == RT_EOK)
    {
        if(PreCtr_Flag == 0)  //心跳
        {
            rt_device_write(serial_4, 0, &dmf, 8);
            rt_kprintf("PreCtr_Flag: %d\n", PreCtr_Flag);
        }
        else if (PreCtr_Flag == 1)
        {
            rt_device_write(serial_4, 0, &dmf, sizeof(dmf));
            rt_kprintf("PreCtr_Flag: %d\n", PreCtr_Flag);
#if DeBug
            rt_kprintf("***send_pressureid: %d\n",dmf.m_pressureid);
            rt_kprintf("***send_cmdtype: %d\n",dmf.m_cmdType);
            rt_kprintf("***send_Ack: %d\n",dmf.m_Ack);
#endif
        }
        else
        {
            rt_kprintf("UART Send is failed\n");
        }

    }
}


