/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
#include "stdint.h"
#include "inc/m_device.h"

#define MAX30205_DEVICEID    "max30205"
#define JFH141_DEVICEID      "jfh141"
#define AFE4300_DEVICEID     "afe4300"

static DeviceObjectDef g_DeviceObject;

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

void manage_device_init(void)
{
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
    if ( pstDeviceObject)
    {
        pstDeviceObject->m_deviceType = emDeviceCompositeSensor;
        char uid[DEVICE_LENGTH];
        get_STM32_uid(uid);
        memset(pstDeviceObject->m_deviceId,0,DEVICE_LENGTH);
        strcpy(pstDeviceObject->m_deviceId,uid);
        pstDeviceObject->m_deviceStatus = 0;
        pstDeviceObject->m_mqttStatus =0;
        pstDeviceObject->m_check_acupointId = 0;
        pstDeviceObject->m_pressValue_param = 0;
    }
}

void manage_device_start(void)
{

}

void manage_device_stop(void)
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

void get_STM32_uid(char * deviceid)
{
    uint32_t id[3];
    MCUTypedef type = STM32F4;
    id[0]=*(uint32_t*)(idAddr[type]);
    id[1]=*(uint32_t*)(idAddr[type]+4);
    id[2]=*(uint32_t*)(idAddr[type]+8);
    char STM32_DEVICEID[DEVICE_LENGTH];
    memset(STM32_DEVICEID ,0 ,DEVICE_LENGTH);
    itoa(id[2],STM32_DEVICEID,10);
    if ( deviceid != SD_NULL)
    {
        memset(deviceid ,0 ,DEVICE_LENGTH);
        strcpy(deviceid ,STM32_DEVICEID);
    }
}
