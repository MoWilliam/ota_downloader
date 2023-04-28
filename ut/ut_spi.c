/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-03-10     Administrator       the first version
 */
#include "inc/ut_spi.h"
#if 0
#include <rtdevice.h>
#include "stm32f4xx_hal.h"
#define AT_SPI_CS GPIO_PIN_11
#define AT_SPI_RSET GPIO_PIN_10

#define AT_SPI_DEVICE_NAME     "spi20"
struct rt_spi_device *at_spi_dev;
struct rt_spi_configuration at_cfg_dev;


void at_SPI_Init(void)
{
    rt_err_t ret;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    ret = rt_hw_spi_device_attach("spi2", AT_SPI_DEVICE_NAME, GPIOB, AT_SPI_CS);
    if ( ret != RT_EOK)
    {
        rt_kprintf("rt_hw_spi_device_attach ERROR %d\n", ret);
    }

    at_spi_dev = (struct rt_spi_device *)rt_device_find(AT_SPI_DEVICE_NAME);
    if (!at_spi_dev)
    {
        rt_kprintf("spi sample run failed! can't find %s device!\n", AT_SPI_DEVICE_NAME);
    }
    else
    {
        at_spi_dev->bus->owner = at_spi_dev;

        at_cfg_dev.data_width = 8;
        at_cfg_dev.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
        at_cfg_dev.max_hz = 1 * 1000 * 1000;
        rt_spi_configure(at_spi_dev, &at_cfg_dev);
    }
}

void at_Reset(void)
{
    int value;
    rt_pin_mode(AT_SPI_RSET, PIN_MODE_OUTPUT);
    rt_pin_write(AT_SPI_RSET, PIN_LOW);
    rt_thread_mdelay(10);
    rt_pin_write(AT_SPI_RSET, PIN_HIGH);
    value = rt_pin_read(AT_SPI_RSET);
    rt_kprintf("pin is:%x %d\n", AT_SPI_RSET, value);
}

void at_spiWrite(unsigned char spiAddr, unsigned short spiData)
{
    rt_err_t ret;
    rt_uint8_t write_addr = spiAddr;
    rt_uint16_t write_data = spiData;
    rt_kprintf("spiWrite is:%x %x\n", spiAddr, spiData);
    ret = rt_spi_send_then_send(at_spi_dev,&write_addr,1,&write_data,2);
    if ( ret != RT_EOK)
    {
        rt_kprintf("rt_spi_send_then_send ERROR %d\n", ret);
    }
}

unsigned short at_spiRead(unsigned char spiAddr)
{
    rt_err_t ret;
    rt_uint8_t read_addr;
    read_addr = read_addr | 0x20; //First 3 bits need to be 001 for read opcode
    unsigned short spiData;
    ret = rt_spi_send_then_recv(at_spi_dev, &read_addr, 1, &spiData, 2);
    if ( ret != RT_EOK)
    {
        rt_kprintf("rt_spi_send_then_recv ERROR %d\n", ret);
    }
    rt_kprintf("spiRead is:%x %x\n", read_addr, spiData);
    return spiData;
}

void bsp_at_init(void)
{
    short afe4300Data_new;
    short afe4300Reg;

    unsigned short i = 5;
    float I_ADC_Value = 0;
    float Q_ADC_Value = 0;

    at_SPI_Init();
    at_Reset();
    rt_thread_mdelay(10);

#if 1
    //FWR
    // 0x01 ADC_CONTROL_REGISTER1 这里很多位都是配置不同功能的
    at_spiWrite(0x01, 0x4100);//0x4170); // 860SPS
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x01));
    at_spiWrite(0x01, 0x4140);
    // DEVICE_CONTROL1 第0位和第2位 和电源相关
    // 开BMP还是体重

    at_spiWrite(0x02, 0x0000); // 空的寄存器
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x02));
    at_spiWrite(0x02, 0x0000);
    at_spiWrite(0x03, 0xFFFF); // 空的寄存器
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x03));
    at_spiWrite(0x03, 0xFFFF);
    at_spiWrite(0x09, 0x0006);//6006);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x09));
    at_spiWrite(0x09, 0x0006);
    // 设置DAC频率     250k
    at_spiWrite(0x0E, 0x0020);//0x00FF);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0E));
    at_spiWrite(0x0E, 0x0020);

    // 开一个电流的通道     0：+ 1：-
    at_spiWrite(0x0A, 0x0408);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0A));
    at_spiWrite(0x0A, 0x0408);
    // 开一个电压测量通道     0：+ 1：-
    at_spiWrite(0x0B, 0x0408);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0B));
    at_spiWrite(0x0B, 0x0408);
    // 分频    IQ_DEMOD_CLK      BCM_DAC_FREQ
    //  1        1M                 250K !
    //  2        500K               125K !
    //  4        250K               62.5K
    //  8        125K               31.25K
    //  16       62.5K              15.625K
    //  32       31.25K             7.8125K
    // 开启FWR模式
    at_spiWrite(0x0C, 0x0000);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0C));
    at_spiWrite(0x0C, 0x0000);
    // IQ_DEMOD_CLK_DIV_FAC:1分频
    at_spiWrite(0x0F, 0x1800);//0000);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0F));
    at_spiWrite(0x0F, 0x1800);
#else
    at_spiWrite(0x01, 0x4130);//0x4170); // 860SPS
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x01));
    at_spiWrite(0x01, 0x4130);
    // DEVICE_CONTROL1 第0位和第2位 和电源相关
    // 开BMP还是体重

    at_spiWrite(0x02, 0x0000); // 空的寄存器
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x02));
    at_spiWrite(0x02, 0x0000);
    at_spiWrite(0x03, 0xFFFF); // 空的寄存器
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x03));
    at_spiWrite(0x03, 0xFFFF);
    at_spiWrite(0x09, 0x0006);//6006);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x09));
    at_spiWrite(0x09, 0x0006);
    // 设置DAC频率     250k
    at_spiWrite(0x0E, 0x0008);//0x00FF);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0E));
    at_spiWrite(0x0E, 0x0008);

    // 开一个电流的通道     0：+ 1：-
    at_spiWrite(0x0A, 0x0201);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0A));
    at_spiWrite(0x0A, 0x0201);
    // 开一个电压测量通道     0：+ 1：-
    at_spiWrite(0x0B, 0x0201);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0B));
    at_spiWrite(0x0B, 0x0201);
    // 分频    IQ_DEMOD_CLK      BCM_DAC_FREQ
    //  1        1M                 250K !
    //  2        500K               125K !
    //  4        250K               62.5K
    //  8        125K               31.25K
    //  16       62.5K              15.625K
    //  32       31.25K             7.8125K
    // 开启IQ模式
    at_spiWrite(0x0C, 0x0800);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0C));
    at_spiWrite(0x0C, 0x0800);
    // IQ_DEMOD_CLK_DIV_FAC:1分频
    at_spiWrite(0x0F, 0x2800);//0000);
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x0F));
    at_spiWrite(0x0F, 0x2800);

#endif
    //    //ADC信号的来源 ，参考电压选择
    //    spiWrite(0x10,0x0063); //I通道
    //    spiWrite(0x10,0x0065); //Q通道
    at_spiWrite(0x1A, 0x0030); // 空的寄存器,总是写0x0030
    rt_kprintf("spiData = %x\r\n",at_spiRead(0x1A));
    at_spiWrite(0x1A, 0x0030);

    rt_kprintf("初始化......\r\n");
    // 下面是测量自己的电阻
    while (i > 1)
    {
        i = i-1;
        // ADC信号的来源 ，参考电压选择
        at_spiWrite(0x10, 0x0063); // I通道
        delay_ms(50);
        afe4300Data_new = at_spiRead(0);
        I_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
         rt_kprintf("VSENSEM_R0 AD I:%d  %f \r\n",afe4300Data_new, I_ADC_Value);

        // ADC信号的来源 ，参考电压选择
         at_spiWrite(0x10, 0x0065); // Q通道
        delay_ms(50);
        afe4300Data_new = at_spiRead(0);
        Q_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
        rt_kprintf("VSENSEM_R0 AD Q:%d  %f \r\n",afe4300Data_new,Q_ADC_Value);
    }
}

#endif
