/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
//#include "stdint.h"
#include "inc/m_device.h"

#define MAX30205_DEVICEID    "max30205"
#define JFH141_DEVICEID      "jfh141"
#define AFE4300_DEVICEID     "afe4300"

static DeviceObjectDef g_DeviceObject;
static PreCtrFrameDef g_PreCtrFrame;

uint32_t idAddr[]={
        0x1FFFF7AC,
                0x1FFFF7E8,  /*STM32F1唯一ID起始地址*/
                0x1FFF7A10,  /*STM32F2唯一ID起始地址*/
                0x1FFFF7AC,  /*STM32F3唯一ID起始地址*/
                0x1FFF7A10,  /*STM32F4唯一ID起始地址*/
                0x1FF0F420,  /*STM32F7唯一ID起始地址*/
                0x1FF80050,  /*STM32L0唯一ID起始地址*/
                0x1FF80050,  /*STM32L1唯一ID起始地址*/
                0x1FFF7590,  /*STM32L4唯一ID起始地址*/
                0x1FF0F420}; /*STM32H7唯一ID起始地址*/

LPDeviceObjectDef device_ctrl_object_get(void)
{
    return &g_DeviceObject;
}

LPPreCtrFrameDef device_prectrl_object_get(void)
{
    return &g_PreCtrFrame;
}

void manage_device_init(void)
{
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
    if ( pstDeviceObject)
    {
        pstDeviceObject->m_deviceType = emDeviceCompositeSensor;
        pstDeviceObject->m_deviceStatus = 0;
        pstDeviceObject->m_mqttStatus =0;
        pstDeviceObject->m_check_acupointId = 0;
        pstDeviceObject->m_pressValue_param = 0;
        pstDeviceObject->m_mqtt_client_isStart = 0;
    }
}

void manage_device_start(void)
{

}

void manage_device_stop(void)
{
    
}

void manage_prectrdevice_init(void)   //气动单元
{
    LPPreCtrFrameDef pstPreCtrFrameDef = device_ctrl_object_get();
    if ( pstPreCtrFrameDef )
    {
        pstPreCtrFrameDef->msgID = 0;
        pstPreCtrFrameDef->m_pressureid = 0;
        pstPreCtrFrameDef->m_msgType = 0;
        pstPreCtrFrameDef->m_deviceType = emDevicePressControlSensor;
        pstPreCtrFrameDef->m_cmdType = 0;
        //pstPreCtrFrameDef->m_deviceStatus = 0;
        
    }
}

void manage_prectrdevice_start(void)
{
    
}
void manage_prectrdevice_stop(void)
{

}

void max30205_deviceid(char * deviceid)
{
    if ( deviceid != SD_NULL)
    {
        memset(deviceid ,0 ,DEVICE_LENGTH);
        strcpy(deviceid ,MAX30205_DEVICEID);
    }
}

int max30205_deviceStatus(void)
{
    return 1;
}

void jfh141_deviceid(char * deviceid)
{
    if ( deviceid != SD_NULL)
    {
        memset(deviceid ,0 ,DEVICE_LENGTH);
        strcpy(deviceid ,JFH141_DEVICEID);
    }
}

int jfh141_deviceStatus(void)
{
    return 1;
}

void afe4300_deviceid(char * deviceid)
{
    if ( deviceid != SD_NULL)
    {
        memset(deviceid ,0 ,DEVICE_LENGTH);
        strcpy(deviceid ,AFE4300_DEVICEID);
    }
}

int afe4300_deviceStatus(void)
{
    return 1;
}

/*uint32_t get_STM32_uid(SdUInt8 * deviceid)
{
    uint8_t i = 0;
    uint32_t id[3];
    MCUTypedef type = STM32f4;
    id[0]=*(uint32_t*)(idAddr[type]);
    id[1]=*(uint32_t*)(idAddr[type]+4);
    id[2]=*(uint32_t*)(idAddr[type]+8);
#if DeBug
    rt_kprintf("ID: %08X-%08X-%08X\n",id[0],id[1],id[2]);
#endif
    //char STM32_DEVICEID[6];
    //memset(STM32_DEVICEID ,0 ,6);
    //itoa(id[2],STM32_DEVICEID,10);    //该函数的含义将整数转换为字符串
    //sprintf(STM32_DEVICEID, "%X", id[0]); //将整数格式化为十六进制字符串，并存储到指定的字符数组中
    if( deviceid != SD_NULL)
    {
        deviceid[0] = (id[0] >> 16) & 0xFF; // 获取高字节
        deviceid[1] = id[0] & 0xFF; // 获取低字节
       // deviceid = STM32_DEVICEID & 0xFFFF;
        //memset(deviceid ,0 ,DEVICE_LENGTH1);
        //memcpy(deviceid, (SdUInt8 *)STM32_DEVICEID, 4);
        //rt_kprintf("deviceId: %s\n", deviceid);
        
    }else
    {
        // 数组长度不够，进行错误处理
        rt_kprintf("Device ID array small!\n");
    }
    return id[0];
}*/


uint32_t get_STM32_uid(SdUInt16 * deviceid)
{
    uint8_t i = 0;
    uint32_t id[3];
    MCUTypedef type = STM32f4;
    id[0]=*(uint32_t*)(idAddr[type]);
    id[1]=*(uint32_t*)(idAddr[type]+4);
    id[2]=*(uint32_t*)(idAddr[type]+8);
#if DeBug
    rt_kprintf("ID: %08X-%08X-%08X\n",id[0],id[1],id[2]);
#endif
    
    deviceid = (SdUInt16) ((id[0] >> 8) & 0xFFFF) | (id[0] & 0xFFFF);

    return deviceid;
}




void get_esp8266_mac(char *macAddr)
{

}

void set_esp8266_mac(rt_uint32_t mac0,rt_uint32_t mac1,rt_uint32_t mac2)
{
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
    if ( pstDeviceObject)
    {
        char uid0[DEVICE_LENGTH] = {0},uid1[DEVICE_LENGTH] = {0},uid2[DEVICE_LENGTH] = {0};
        char uid[DEVICE_LENGTH] = {0};
        sprintf(uid0,"%x",mac0);
        sprintf(uid1,"%x",mac1);
        sprintf(uid2,"%x",mac2);
        strcpy(uid,uid0);
        strcpy(uid + 2,uid1);
        strcpy(uid + 4,uid2);
        memset(pstDeviceObject->m_deviceId,0,DEVICE_LENGTH);
        strcpy(pstDeviceObject->m_deviceId,uid);
    }
}

SdUInt8 calcCRC(SdUInt8 *data,SdUInt32 len)  //计算CRC码
{
	SdUInt8 crc8 = 0;

    while (len --)
    {
        crc8 = crc8Table[crc8 ^ (*data++)];
        
    }

    return crc8;
}

SdUInt8 get_DEVSoftEdition(void){
#if COMPOSITE_CONTROL_FLAG
SdUInt8 ComSorVersion[4] = ComSorVer;   //当前综采软件版本号

return PreCtrVersion;
#endif

#if PRESS_CONTROL_FLAG
SdUInt8 PreCtrVersion[4] = PreCtrVer;  //当前气动单元软件版本号

return PreCtrVersion;
#endif


}
