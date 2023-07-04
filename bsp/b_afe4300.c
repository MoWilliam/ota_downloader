/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */

#include "inc/b_afe4300.h"
#include "inc/spi.h"

#include <math.h>

#define FWR_MODEL 0

void delay_ms(int ms)
{
    rt_thread_mdelay(ms);
}

/**
 *kalman_init - 卡尔曼滤波器初始化
 *@kalman_lcw：卡尔曼滤波器结构体
 *@init_x：待测量的初始值
 *@init_p：后验状态估计值误差的方差的初始值
 */
void kalman_init(kalman_struct *kalman_lcw, float init_x, float init_p)
{
    kalman_lcw->x = init_x; // 待测量的初始值，如有中值一般设成中值（如陀螺仪）
    kalman_lcw->p = init_p; // 后验状态估计值误差的方差的初始值
    kalman_lcw->A = 1;
    kalman_lcw->H = 1;
    kalman_lcw->q = 10e-3; // 10e-6;//2e2;////predict noise convariance 预测（过程）噪声方差 实验发现修改这个值会影响收敛速率
    kalman_lcw->r = 5e2;   // 10e-5;//测量（观测）噪声方差。以陀螺仪为例，测试方法是：
    // 保持陀螺仪不动，统计一段时间内的陀螺仪输出数据。数据会近似正态分布，
    // 按3σ原则，取正态分布的(3σ)^2作为r的初始化值
}

/**
 *kalman_filter - 卡尔曼滤波器
 *@kalman_lcw:卡尔曼结构体
 *@measure；测量值
 *返回滤波后的值
 */
float kalman_filter(kalman_struct *kalman_lcw, float measure)
{
    /* Predict */
    kalman_lcw->x = kalman_lcw->A * kalman_lcw->x;
    kalman_lcw->p = kalman_lcw->A * kalman_lcw->A * kalman_lcw->p + kalman_lcw->q; /* p(n|n-1)=A^2*p(n-1|n-1)+q */

    /* Measurement */
    kalman_lcw->gain = kalman_lcw->p * kalman_lcw->H / (kalman_lcw->p * kalman_lcw->H * kalman_lcw->H + kalman_lcw->r);
    kalman_lcw->x = kalman_lcw->x + kalman_lcw->gain * (measure - kalman_lcw->H * kalman_lcw->x);
    kalman_lcw->p = (1 - kalman_lcw->gain * kalman_lcw->H) * kalman_lcw->p;

    return kalman_lcw->x;
}


/**
 *AFE4300_Reset - reset AFE4300的IO口
 *GPIO: GPIO_Pin_14
 *GPIO_G:
 */
void AFE4300_Reset(void)
{
//     GPIO_ResetBits(GPIOG, GPIO_Pin_14);
//     delay_ms(1);
//     GPIO_SetBits(GPIOG, GPIO_Pin_14);
    SPI_AFE4300_RESET = 0;
    rt_thread_mdelay(10);
    SPI_AFE4300_RESET = 1;
}

/**
 *spiWrite - 写AFE4300寄存器
 *@spiAddr：寄存器地址
 *@spiData：16位的数据
 *@先发送地址，再发送数据高8，低8
 */
void spiWrite(unsigned char spiAddr, unsigned short spiData)
{

//    SPI_AFE4300_CS = 1;
    SPI_AFE4300_CS = 0; // 使能器件

    SPI2_ReadWriteByte(spiAddr);

    SPI2_ReadWriteByte(spiData >> 8); //   Load MSB write data

    SPI2_ReadWriteByte(spiData); // Load LSB write data

    SPI_AFE4300_CS = 1;
}

/**
 *spiRead - 读取AFE4300寄存器
 *@spiAddr：寄存器地址
 *返回unsigned short 类型的数据(16位)
 */
unsigned short spiRead(unsigned char spiAddr)
{
    unsigned short spiData = 0;
//    SPI_AFE4300_CS = 1;
    SPI_AFE4300_CS = 0; // 使能器件

    SPI2_ReadWriteByte(  0x20 | spiAddr); // 发送读取状态寄存器命令,返回读取到的值 现在这个是没用的  0x20 |

    spiData = (SPI2_ReadWriteByte(0xFF)) << 8; // Read MSB data

    spiData |= SPI2_ReadWriteByte(0xFF); // Read LSB data

    SPI_AFE4300_CS = 1; // 取消片选

//    spiWrite(spiAddr, spiData); // Writeback read data due to feature bug on the BCM device

    return spiData; // Return SPI read data
}

void afe4300_init(void)
{
    short afe4300Data_new;
    uint32_t ret;
    float angle = 0;
    float z = 0;
    // 这两个值用于保存原来的偏移
    float angle_offset = 0;
    float z_offset = 0;
    float angle_sum = 0;
    float z_sum = 0;
    float k = 2.0; // 系数，需要测量确定
    unsigned short i = 5;
    float I_ADC_Value = 0;
    float Q_ADC_Value = 0;
    kalman_struct kalman_angle; // 卡尔曼角度滤波器
    kalman_struct kalman_z;     // 卡尔曼阻抗滤波器

    // Reset AFE4300
    AFE4300_Reset();
/***************************************************
#if FWR_MODEL
    //FWR
    // 0x01 ADC_CONTROL_REGISTER1 这里很多位都是配置不同功能的
    spiWrite(0x01, 0x4100);//0x4170); // 860SPS
    rt_kprintf("spiData = %x\r\n",spiRead(0x01));
    spiWrite(0x01, 0x4140);
    // DEVICE_CONTROL1 第0位和第2位 和电源相关
    // 开BMP还是体重

    spiWrite(0x02, 0x0000); // 空的寄存器
    rt_kprintf("spiData = %x\r\n",spiRead(0x02));
    spiWrite(0x02, 0x0000);
    spiWrite(0x03, 0xFFFF); // 空的寄存器
    rt_kprintf("spiData = %x\r\n",spiRead(0x03));
    spiWrite(0x03, 0xFFFF);
    spiWrite(0x09, 0x0006);//6006);
    rt_kprintf("spiData = %x\r\n",spiRead(0x09));
    spiWrite(0x09, 0x0006);
    // 设置DAC频率	   250k
    spiWrite(0x0E, 0x0020);//0x00FF);
    rt_kprintf("spiData = %x\r\n",spiRead(0x0E));
    spiWrite(0x0E, 0x0020);

    // 开一个电流的通道     0：+ 1：-
    spiWrite(0x0A, 0x0408);
    rt_kprintf("spiData = %x\r\n",spiRead(0x0A));
    spiWrite(0x0A, 0x0408);
    // 开一个电压测量通道	 0：+ 1：-
    spiWrite(0x0B, 0x0408);
    rt_kprintf("spiData = %x\r\n",spiRead(0x0B));
    spiWrite(0x0B, 0x0408);
    // 分频    IQ_DEMOD_CLK      BCM_DAC_FREQ
    //  1		 1M					250K !
    //  2		 500K				125K !
    //  4		 250K				62.5K
    //  8		 125K				31.25K
    //  16		 62.5K				15.625K
    //  32		 31.25K				7.8125K
    // 开启FWR模式
    spiWrite(0x0C, 0x0000);
    rt_kprintf("spiData = %x\r\n",spiRead(0x0C));
    spiWrite(0x0C, 0x0000);
    // IQ_DEMOD_CLK_DIV_FAC:1分频
    spiWrite(0x0F, 0x1800);//0000);
    rt_kprintf("spiData = %x\r\n",spiRead(0x0F));
    spiWrite(0x0F, 0x1800);
    spiWrite(0x1A, 0x0030); // 空的寄存器,总是写0x0030
    rt_kprintf("spiData = %x\r\n",spiRead(0x1A));
    spiWrite(0x1A, 0x0030);
#else
********************************************************/
    spiWrite(0x01, 0x4170);//0x4170); // 860SPS
    rt_kprintf("spiData = %x\r\n",spiRead(0x01));
    spiWrite(0x01, 0x4170);
    // DEVICE_CONTROL1 第0位和第2位 和电源相关
    // 开BMP还是体重

    spiWrite(0x02, 0x0000); // 空的寄存器
    rt_kprintf("spiData = %x\r\n",spiRead(0x02));
    spiWrite(0x02, 0x0000);
    spiWrite(0x03, 0xFFFF); // 空的寄存器
    rt_kprintf("spiData = %x\r\n",spiRead(0x03));
    spiWrite(0x03, 0xFFFF);
    spiWrite(0x09, 0x6006);//6006);
    rt_kprintf("spiData = %x\r\n",spiRead(0x09));
    spiWrite(0x09, 0x6006);
    // 设置DAC频率     250k
    spiWrite(0x0E, 0x0040);//0x00FF);
    rt_kprintf("spiData = %x\r\n",spiRead(0x0E));
    spiWrite(0x0E, 0x0040);

    // 开一个电流的通道     0：+ 1：-
    //spiWrite(0x0A, 0x0201);
    spiWrite(0x0A, 0x0804);   //0X0A地址写电流通道（2：7：电流输入位选择；10：15电流输出选择）
    rt_kprintf("spiData = %x\r\n",spiRead(0x0A));
    //spiWrite(0x0A, 0x0201);
    spiWrite(0x0A, 0x0804);
    // 开一个电压测量通道     0：+ 1：-
    //spiWrite(0x0B, 0x0201);
    spiWrite(0x0B, 0x0804); //0X0B地址写电压通道（2：7：电压输入位选择；10：15电压输出选择）
    rt_kprintf("spiData = %x\r\n",spiRead(0x0B));
    //spiWrite(0x0B, 0x0201);
    spiWrite(0x0B, 0x0804);
    // 分频    IQ_DEMOD_CLK      BCM_DAC_FREQ
    //  1        1M                 250K !
    //  2        500K               125K !
    //  4        250K               62.5K
    //  8        125K               31.25K
    //  16       62.5K              15.625K
    //  32       31.25K             7.8125K
    // 开启IQ模式
    spiWrite(0x0C, 0x0800);
    rt_kprintf("spiData = %x\r\n",spiRead(0x0C));
    spiWrite(0x0C, 0x0800);
    // IQ_DEMOD_CLK_DIV_FAC:1分频
    spiWrite(0x0F, 0x1000);//0000);
    rt_kprintf("spiData = %x\r\n",spiRead(0x0F));
    spiWrite(0x0F, 0x1000);
    spiWrite(0x1A, 0x0030); // 空的寄存器,总是写0x0030
    rt_kprintf("spiData = %x\r\n",spiRead(0x1A));
    spiWrite(0x1A, 0x0030);
//#endif

    //kalman_lcw：卡尔曼滤波器结构体
    //init_x：待测量的初始值
    //init_p：后验状态估计值误差的方差的初始值 5e2
    //kalman_init(&kalman_angle, 30, 2); // 初始化卡尔曼角度滤波器
    //kalman_init(&kalman_z, 30, 2);     // 初始化卡尔曼阻抗滤波器
    // 这两个参数其实不打紧要，最重要的是q和r
    rt_kprintf("init....................................................\r\n");
    while (0)
    {
        // 下面是测量自己的电阻
        while (i > 1)
        {
            i--;
//            rt_kprintf("iiiiiiiiiiiiiiii=%d\r\n",i);
            // ADC信号的来源 ，参考电压选择
            spiWrite(0x10, 0x0063); // I通道  0x0063
            rt_kprintf("spiData = %x\r\n",spiRead(0x10));
            spiWrite(0x10, 0x0063);
            delay_ms(1);
            afe4300Data_new = spiRead(0);
            rt_kprintf("1.spiData0x00 = %x\r\n",afe4300Data_new);
            I_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
//            ret = (uint8_t)I_ADC_Value * 10000000;
            rt_kprintf("VSENSEM_R0 AD I:%f \r\n",I_ADC_Value);

            // ADC信号的来源 ，参考电压选择
            spiWrite(0x10, 0x0065); // Q通道
            delay_ms(1);
            afe4300Data_new = spiRead(0);
            rt_kprintf("2.spiData0x00 = %x\r\n",afe4300Data_new);
            Q_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
//            ret = I_ADC_Value * 10000000;
             rt_kprintf("VSENSEM_R0 AD Q:%f\r\n",Q_ADC_Value);

            // 计算Z和角度θ
//            angle_offset = 57.3 * atan(Q_ADC_Value / I_ADC_Value);
//            angle_offset = kalman_filter(&kalman_angle,angle_offset);
            z_offset = (1 / k) * sqrt(I_ADC_Value * I_ADC_Value + Q_ADC_Value * Q_ADC_Value);
            z_offset = kalman_filter(&kalman_z, z_offset);

            if (i <= 101) // 1到101
            {
//                angle_sum += angle_offset;
                z_sum += z_offset;
            }
            // 这里我直接用最后一组数据作为偏置，因为卡尔曼滤波会收敛，越靠后的值好像越可靠
        }
        if (i == 1)
        {
            i = 0;
//            angle_offset = angle_sum / 100;
            z_offset = z_sum / 100;
            rt_kprintf("init over angle_offset:%f z_offset:%f\r\n", angle_offset, z_offset);
        }
        // ADC信号的来源 ，参考电压选择
        spiWrite(0x10, 0x0063); // I通道
        delay_ms(50);
        afe4300Data_new = spiRead(0);
        I_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
        rt_kprintf("VSENSEM_R0 AD I:%d  %f \r\n",afe4300Data_new, I_ADC_Value);

        // ADC信号的来源 ，参考电压选择
        spiWrite(0x10, 0x0065); // Q通道
        delay_ms(50);
        afe4300Data_new = spiRead(0);
        Q_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
        rt_kprintf("VSENSEM_R0 AD Q:%d  %f \r\n",afe4300Data_new,Q_ADC_Value);

        // 计算Z和角度θ
        //angle = 57.3 * atan(Q_ADC_Value / I_ADC_Value);
        //rt_kprintf("angle=%f\r\n",angle);
        //angle = kalman_filter(&kalman_angle,angle) - angle_offset;//;//卡尔曼滤波结果再减去偏置
        //rt_kprintf("angle=%f\r\n",fabs(angle));
        z = (1 / k) * sqrt(I_ADC_Value * I_ADC_Value + Q_ADC_Value * Q_ADC_Value);
        //rt_kprintf("kalman z=%f\r\n",z);
        //z = kalman_filter(&kalman_z, z) - z_offset; //;//卡尔曼滤波结果再减去偏置
        //rt_kprintf("%f  %f\r\n",angle,z); //θ转为角度了
        //z = 4.8128 * z + 18.0136;
        rt_kprintf("zzzzzzzzzzzzzzzzzzz%f\r\n", z);
        rt_kprintf("******test over******\r\n");

        delay_ms(100);
    }
}

void bsp_afe4300_init(void)
{
    SPI2_Init();
    rt_thread_mdelay(10);
    afe4300_init();
}

void bsp_afe4300_get(BioFrameDef* dmf)
{
    short afe4300Data_new = 0;
    float I_ADC_Value = 0;
    float Q_ADC_Value = 0;
    float k = 2.0; // 系数，需要测量确定
    float z;

    // ADC信号的来源 ，参考电压选择
    spiWrite(0x10, 0x0063); // I通道
    delay_ms(50);
    afe4300Data_new = spiRead(0);
    I_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
    rt_kprintf("VSENSEM_R0 AD I:%d  %f\n",afe4300Data_new, I_ADC_Value);

    // ADC信号的来源 ，参考电压选择
    spiWrite(0x10, 0x0065); // Q通道
    delay_ms(50);
    afe4300Data_new = spiRead(0);
    Q_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
    rt_kprintf("VSENSEM_R0 AD Q:%d  %f\n",afe4300Data_new,Q_ADC_Value);

    // 计算Z和角度θ
    //angle = 57.3 * atan(Q_ADC_Value / I_ADC_Value);
    //rt_kprintf("angle=%f\r\n",angle);
    //angle = kalman_filter(&kalman_angle,angle) - angle_offset;//;//卡尔曼滤波结果再减去偏置
    //rt_kprintf("angle=%f\r\n",fabs(angle));
    z = (1 / k) * sqrt(I_ADC_Value * I_ADC_Value + Q_ADC_Value * Q_ADC_Value);
    //rt_kprintf("kalman z=%f\r\n",z);
    //z = kalman_filter(&kalman_z, z) - z_offset; //;//卡尔曼滤波结果再减去偏置
    //rt_kprintf("%f  %f\r\n",angle,z); //θ转为角度了
    //z = 4.8128 * z + 18.0136;
    //rt_kprintf("zzzzzzzzzzzzzzzzzzz%f\r\n", z);
    //rt_kprintf("******test over******\r\n");
    dmf->m_bio_value = z;
}
