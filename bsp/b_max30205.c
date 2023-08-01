/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-15     ZHZH       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "inc/b_max30205.h"

#define MAX30205_I2C_BUS_NAME          "i2c1"  /* 传感器连接的I2C总线设备名称 */
#define MAX30205_ADDR                  0x48    /* 从机地址 */
#define MAX30205_GET_TEMP              0x00    /* 获取数据命令 */

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;     /* I2C总线设备句柄 */
static rt_bool_t initialized = RT_FALSE;                /* 传感器初始化状态 */

static double p_last = 0;
static double x_last = 0;

//过程噪音
#define P_Q 0.1  // Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
//测量噪声
#define M_R 0.01 //R:测量噪声，R增大，动态响应变慢，收敛稳定性变好

/********卡拉曼滤波************/
extern float kalman_filter_temp(float inData)
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
/* 写传感器寄存器 */
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data)
{
    rt_uint8_t buf[3];
    struct rt_i2c_msg msgs;

    buf[0] = reg; //cmd
    buf[1] = data[0];
    buf[2] = data[1];

    msgs.addr = MAX30205_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = 3;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

/* 读传感器寄存器数据 */
static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)
{
    struct rt_i2c_msg msg[2];

//    msgs.addr = MAX30205_ADDR;
//    msgs.flags = RT_I2C_RD;
//    msgs.buf = buf;
//    msgs.len = len;

    msg[0].addr  = MAX30205_ADDR;
    msg[0].flags = RT_I2C_WR;
    msg[0].len   = 1;
    msg[0].buf   = &reg;
    msg[1].addr  = MAX30205_ADDR;
    msg[1].flags = RT_I2C_RD;
    msg[1].len   = len;
    msg[1].buf   = buf;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msg, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        rt_kprintf("i2c bus read failed!\r\n");
        return -RT_ERROR;
    }
}

static void read_temp_humi(DataFrameDef* dmf)
{
    rt_uint8_t temp[2];

//    write_reg(i2c_bus, MAX30205_GET_TEMP, 0);      /* 送命令 */
    read_regs(i2c_bus, MAX30205_GET_TEMP, 2, temp);                /* 获取传感器数据 */

//    rt_kprintf("temp[0] = %d\r\n",temp[0]);
//    rt_kprintf("temp[1] = %d\r\n",temp[1]);

    dmf->m_atemp = temp[0];
    dmf->m_btemp = temp[1];

}

static void max30205_init(const char *name)
{
    rt_uint8_t temp[2] = {0, 0};

    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(name);

    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", name);
    }
    else
    {
//        write_reg(i2c_bus, AHT10_NORMAL_CMD, temp);
//        rt_thread_mdelay(400);
//
//        temp[0] = 0x08;
//        temp[1] = 0x00;
//        write_reg(i2c_bus, AHT10_CALIBRATION_CMD, temp);
//        rt_thread_mdelay(400);
        read_regs(i2c_bus, MAX30205_GET_TEMP, 2, temp);                /* 获取传感器数据 */

        rt_kprintf("*******temp[0] = %d\r\n",temp[0]);
        rt_kprintf("*******temp[1] = %d\r\n",temp[1]);
        rt_thread_mdelay(400);
        initialized = RT_TRUE;
    }
}

void bsp_max30205_init(void)
{
    char name[RT_NAME_MAX];
    /* 传感器初始化 */
//    aht10_init(name);
    rt_strncpy(name, MAX30205_I2C_BUS_NAME, RT_NAME_MAX);
    if (!initialized)
    {
        /* 传感器初始化 */
        max30205_init(name);
    }
}
void bsp_max30205_get(DataFrameDef* dmf)
{
//    if (initialized)
//    {
        read_temp_humi(dmf);
//    }
//    else
//    {
//        rt_kprintf("initialize sensor failed!\n");
//    }
}
