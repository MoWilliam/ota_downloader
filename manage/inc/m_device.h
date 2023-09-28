/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
#ifndef MANAGER_M_DEVICE_H_
#define MANAGER_M_DEVICE_H_

#include "application.h"

#define DEVICE_LENGTH 32

typedef enum TagMCUTypedef{
  STM32F0,
  STM32F1,
  STM32F2,
  STM32F3,
  STM32F4,
  STM32F7,
  STM32L0,
  STM32L1,
  STM32L4,
  STM32H7,
}MCUTypedef;


typedef enum TagDeviceTypeDef
{
    emDeviceUnknown,
    emDeviceCompositeSensor,
    emDevicePressSensor,
    emDevicePressControlSensor

}deviceTypeDef;

/**
 * data struct object
 *
 */

typedef struct TagDataFrameDef
{
    SdUInt16 msgID;
    char m_deviceid[DEVICE_LENGTH];
    SdUInt8 m_deviceStatus;
    SdUInt16 m_atemp;
    SdUInt16 m_btemp;

}DataFrameDef,*LPDataFrameDef;

typedef struct TagSpo2FrameDef
{
    SdUInt16 msgID;
    char m_deviceid[DEVICE_LENGTH];
    SdUInt8 m_deviceStatus;
    SdUInt16 m_spo2;
    SdUInt16 m_bk;

}Spo2FrameDef,*LPSpo2FrameDef;

typedef struct TagBioFrameDef
{
    SdUInt16 msgID;
    char m_deviceid[DEVICE_LENGTH];
    SdUInt8 m_deviceStatus;
    SdFloat m_bio_voltage;
    SdFloat m_bio_ampere;
    SdUInt16 m_bio_value;

}BioFrameDef,*LPBioFrameDef;


//2023.9.27尝试用一个数组安装传感器数据

typedef struct TagSensorDataFrameDef
{
    SdUInt16 msgID;
    char m_deviceid[DEVICE_LENGTH];
    SdUInt8 m_deviceStatus;
    //data-temp
    SdUInt16 m_atemp;
    SdUInt16 m_btemp;
    //data-spo2
    SdUInt16 m_spo2;
    SdUInt16 m_bk;
    //data-bio
    SdFloat m_bio_voltage;
    SdFloat m_bio_ampere;
    SdUInt16 m_bio_value;

}SensorDataFrameDef,*LPSensorDataFrameDef;


/**
 * device struct object
 *
 */
typedef struct TagDeviceObjectDef
{
    char m_deviceId[DEVICE_LENGTH];
    SdUInt8 m_deviceType;
    SdUInt8 m_deviceStatus; //0:stop work,1:start work
    SdUInt8 m_mqttStatus; // 0:offline ,1:online,2:connected
    SdUInt8 m_pressValue_param;
    SdUInt16 m_check_acupointId;
    SdInt8 m_mqtt_client_isStart;
    SdInt8 m_device_collect;//0:未采集，1:采集（根据血氧数据来判断）

}DeviceObjectDef,*LPDeviceObjectDef;

typedef struct TagPreCtrmqFrameDef    //uart接收消息队列
{
    SdUInt16 msgID;
    SdUInt8 m_presorID;  //所需要控制加压设备
    SdUInt8 m_cmdtype;  //加压和泄压命令

}PreCtrmqFrameDef,*LPPreCtrmqFrameDef;

typedef struct TagPreCtrFrameDef   //心跳包消息队列,uart发送消息队列
{
    SdUInt16 msgID;
    char m_deviceid[DEVICE_LENGTH];
    SdUInt8 m_deviceType;
    SdUInt8 m_deviceStatus;

    //SdUInt8 m_presorID;        设备号码 设置一个组出来z
    //SdUInt8 m_prectrcmdtype;   0,1状态位
}PreCtrFrameDef,*LPPreCtrFrameDef;

typedef enum TagPrectrDevList { 
    PressureSensor1, 
    PressureSensor2,
    PressureSensor3, 
    PressureSensor4, 
    PressureSensor5
}prectrDevList;    //设定一个设备id的数组

LPDeviceObjectDef device_ctrl_object_get(void);

/*STM32 uid*/
void get_STM32_uid(char * deviceid);
void get_esp8266_mac(char *macAddr);
void set_esp8266_mac(rt_uint32_t mac0,rt_uint32_t mac1,rt_uint32_t mac2);

void manage_device_init(void);
void manage_device_start(void);
void manage_device_stop(void);

void max30205_deviceid(char * deviceid);
int max30205_deviceStatus(void);
void jfh141_deviceid(char * deviceid);
int jfh141_deviceStatus(void);
void afe4300_deviceid(char * deviceid);
int afe4300_deviceStatus(void);

void manage_prectrdevice_init(void);
void manage_prectrdevice_start(void);
void manage_prectrdevice_stop(void);

#endif /* MANAGER_M_DEVICE_H_ */
