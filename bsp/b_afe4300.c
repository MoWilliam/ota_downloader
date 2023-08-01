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

static double p_last = 0;
static double x_last = 0;

//综采设备选择
#define e2b8d6 0
#define e1ee32 0
#define e22bb7 0
#define e28771 0
#define e22526 0
#define e27275 0
#define e7aa1b 0
#define e271fd 0
#define e2336c 0
#define e2eb70 0
#define e7e033 0
#define e22a8a 0
#define e22635 0
#define e2bded 0
#define e1f1c6 1
#define e2c4a7 0
#define e23035 0

//过程噪音
#define P_Q 0.2  // Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
//测量噪声
#define M_R 0.01 //R:测量噪声，R增大，动态响应变慢，收敛稳定性变好




void delay_ms(int ms)
{
    rt_thread_mdelay(ms);
}

/********卡拉曼滤波************/
float kalman_filter(float inData)
{
  static float prevData=0;

  static float p=0.1, q=P_Q, r=M_R, kGain=0; //其中p的初值可以随便取，但是不能为0（为0的话卡尔曼滤波器就认为已经是最优滤波器了）
    p = p+q;
    kGain = p/(p+r);

    inData = prevData+(kGain*(inData-prevData));
    p = (1-kGain)*p;

    prevData = inData;

    return inData;
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
    //kalman_struct kalman_angle; // 卡尔曼角度滤波器
    //kalman_struct kalman_z;     // 卡尔曼阻抗滤波器

    // Reset AFE4300
    AFE4300_Reset();

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
    spiWrite(0x0E, 0x0040);//0x00FF)原始频率
    //spiWrite(0x0E, 0x0001);    //3.9k
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
#endif

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
            //angle_offset = 57.3 * atan(Q_ADC_Value / I_ADC_Value);
            //angle_offset = kalman_filter(&kalman_angle,angle_offset);
            z_offset = (1 / k) * sqrt(I_ADC_Value * I_ADC_Value + Q_ADC_Value * Q_ADC_Value);
            //z_offset = kalman_filter(&kalman_z, z_offset);

            if (i <= 101) // 1到101
            {
                //angle_sum += angle_offset;
                z_sum += z_offset;
            }
            // 这里我直接用最后一组数据作为偏置，因为卡尔曼滤波会收敛，越靠后的值好像越可靠
        }
        if (i == 1)
        {
            i = 0;
            //angle_offset = angle_sum / 100;
            z_offset = z_sum / 100;
            rt_kprintf("init over angle_offset:%f z_offset:%f\r\n", angle_offset, z_offset);
        }
        // ADC信号的来源 ，参考电压选择
        spiWrite(0x10, 0x0063); // I通道
        delay_ms(50);
        afe4300Data_new = spiRead(0);
        I_ADC_Value = afe4300Data_new * 0.051889; // AD值转化为电压
        //rt_kprintf("VSENSEM_R0 AD I:%d  %f \r\n",afe4300Data_new, I_ADC_Value);

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
        rt_kprintf("kalman z=%f\r\n",z);
        //rt_kprintf("%f  %f\r\n",angle,z); //θ转为角度了
        z = 4.8128 * z + 18.0136;
        rt_kprintf("kalman z=%f\r\n",z);
        rt_kprintf("zzzzzzzzzzzzzzzzzzz%f\r\n", z);

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
    float z = 0;
    short afe4300Data_new = 0;
    float I_ADC_Value = 0;
    float Q_ADC_Value = 0;
    float k = 0.67761;  // 系数，需要测量确定
    //float k = 0.6710;  //校正后k参数（针对于低阻抗数值）

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

    z = (1 / k) * sqrt(I_ADC_Value * I_ADC_Value + Q_ADC_Value * Q_ADC_Value) ;
    z= 1.01 * z + 0.31; //校正后的

#if e2b8d6
    z = 0.99 * z + 13.57;
#endif

#if e1ee32
    z = 1.01 *z -12.06;

#endif

#if e22bb7
    z = 0.99 * z + 2.38;
#endif

#if e28771
    z= 0.99 * z + 0.94;
#endif

#if e22526
    z= z + 7;
#endif

#if e27275
    z= z + 4;
#endif

#if e7aa1b
    z= 1.01 * z + 2.84;
#endif

#if e271fd
    z= z + 4.25;
#endif

#if e2336c
    z= 1.01 * z - 4.48;
#endif

#if e2eb70
    z= z - 3;
#endif

#if e7e033
    z= 1.01 * z - 4.48;
#endif

#if e22a8a
    z= 0.99 * z - 2.02;
#endif

#if e22635
    z= z + 11;
#endif

#if e2bded
    z= z - 7;
#endif

#if e1f1c6
    z= 0.96 * z + 5.95;
#endif

#if e2c4a7
    z= z + 10;
#endif

#if e23035
    z= 1.01 * z + 3.34;
#endif


    //rt_kprintf("z=%f\r\n", z);
    z = kalman_filter(z); //;//做卡尔曼滤波
    //rt_kprintf("kalman z=%f\r\n",z);

    dmf->m_bio_value = z;
}
