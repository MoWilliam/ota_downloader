/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-13     ZHZH       the first version
 */
#include <rtthread.h>
#include "stdint.h"
#include "string.h"
#include "bsp/inc/b_jfh141.h"
#include <rtdevice.h>

 /* 串口设备名称 不能使用UART2 ,需要重新分配*/

#define SAMPLE_UART_NAME       "uart3"

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
static rt_device_t serial;

/*判断是否读取一组数据结束的标志位*/
uint8_t rwflag = 0;
uint8_t data[128] = {0};

//卡尔曼滤波
static double p_last = 0;
static double x_last = 0;

//过程噪音
#define P_Q 0.1  // Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
//测量噪声
#define M_R 0.05 //R:测量噪声，R增大，动态响应变慢，收敛稳定性变好

/********卡拉曼滤波************/
extern float kalman_filter_jfh141(float inData)
{
  static float prevData=0;

  static float p=0.01, q=P_Q, r=M_R, kGain=0; //其中p的初值可以随便取，但是不能为0（为0的话卡尔曼滤波器就认为已经是最优滤波器了）
    p = p+q;
    kGain = p/(p+r);

    inData = prevData+(kGain*(inData-prevData));
    p = (1-kGain)*p;

    prevData = inData;

    return inData;
}

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void vital_signs_analysis(void)
{
    rt_kprintf("data[0] = %d\r\n",data[0]);
    rt_kprintf("data[1] = %d\r\n",data[1]);
    rt_kprintf("data[64] = %d\r\n",data[64]);
    rt_kprintf("data[65] = %d\r\n",data[65]);
    rt_kprintf("data[66] = %d\r\n",data[66]);
    rt_kprintf("data[71] = %d\r\n",data[71]);
    rt_kprintf("data[72] = %d\r\n",data[72]);
}


void bsp_jfh141_init(void)
{
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
//    char str[] = "hello RT-Thread!\r\n";
    uint8_t c_on = 0x8A; //0x88; 采集关

    rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);
    /* 查找串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate = BAUD_RATE_38400;
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    /* 以读写及中断接收方式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);
    /* 发送字符串 */
    rt_device_write(serial, 0, &c_on, 1);
}


//void bsp_jfh141_get(Spo2FrameDef* dmf)
void bsp_jfh141_get(SensorDataFrameDef* dmf) //2023.9.27
{
    char ch;
    SdInt16 last_m_spo2 ;   //上次血氧获得的有效值。
    SdInt16 last_m_bk ;     //上次微循环获得的有效值。

    while (1)
    {
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(serial, -1, &ch, 1) != 1)
        {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
        if(ch != 0xFF) //实时包以0xFF打头，数据包中不会出现其他0xFF数据
        {
            data[rwflag++] = ch;
            if(rwflag >= 76)
            {
                //vital_signs_analysis();
                //心率：66位;血氧：67位;微循环68位
                //dmf->m_spo2 = data[66];
                //dmf->m_bk = data[67];

                //判断血氧传感器是否进行检测物体
                if (data[1] != 0xC4 && data[63] != 0xC4){
                    //rt_kprintf("111111");
                    if( data[66] > 0 && data[66] < 101 ){
                        
                        //dmf->m_object_spo2_detected = RT_TRUE;  //表明血氧传感器在工作
                        last_m_spo2 = data[66];  //更新上次检测的血氧有效值
                        dmf->m_spo2 = data[66];

                        
                    }else {
                        if(last_m_spo2 > 0 && last_m_spo2 < 101)
                             dmf->m_spo2 = last_m_spo2;

                        //dmf->m_object_spo2_detected = RT_FALSE;
                        //object_bk_detected = RT_FALSE;
                    }

                    if(data[67] > 0 && data[67] < 100) {
                        //dmf->m_object_bk_detected = RT_TRUE;
                        last_m_bk = data[67];      //更新上次检测的微循环有效值
                        dmf->m_bk = data[67];
                    }else{

                        if(last_m_bk > 0 && last_m_bk <100 )
                           dmf->m_bk = last_m_bk;
                        //dmf->m_object_bk_detected = RT_FALSE;

                    }
                       
                }else {
                    dmf->m_spo2 = 0;
                    dmf->m_bk = 0;
                    last_m_spo2 = 0;
                    last_m_bk = 0;

                }

                rwflag = 0;
                break;
            }
            
        }
        else{
            memset(data, 0, sizeof(data));
            rwflag = 0;
            data[rwflag++] = ch;

        }

    }

}



