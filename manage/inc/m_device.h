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
    /* data */
    SdUInt16 m_atemp;
    SdUInt16 m_btemp;

}DataFrameDef,*LPDataFrameDef;

typedef struct TagSpo2FrameDef
{
    SdUInt16 msgID;
    char m_deviceid[DEVICE_LENGTH];
    SdUInt8 m_deviceStatus;
    /* data */
    SdUInt16 m_spo2;

}Spo2FrameDef,*LPSpo2FrameDef;

typedef struct TagBioFrameDef
{
    SdUInt16 msgID;
    char m_deviceid[DEVICE_LENGTH];
    SdUInt8 m_deviceStatus;
    /* data */
    SdFloat m_bio_voltage;
    SdFloat m_bio_ampere;
    SdUInt16 m_bio_value;

}BioFrameDef,*LPBioFrameDef;

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

}DeviceObjectDef,*LPDeviceObjectDef;

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

#endif /* MANAGER_M_DEVICE_H_ */