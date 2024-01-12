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
#include "stdint.h"
#include "application.h"

#define DEVICE_LENGTH 32
#define DEVICE_LENGTH1 2

#define ComSorVer   "V1.4"
#define PreCtrVer   "V1.2"

typedef enum TagMCUTypedef{
  STM32F0,
  STM32F1,
  STM32F2,
  STM32F3,
  STM32f4,
  STM32F7,
  STM32L0,
  STM32L1,
  STM32L4,
  STM32H7,
}MCUTypedef;

static const unsigned int crc8Table[256] =
{
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};                        //crc校验，按照多项式 X^8+X^2+X^1+1 生成。

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
typedef struct TagSensorDataFrameDef
{
    SdUInt16 msgID;
    char m_deviceid[DEVICE_LENGTH];
    SdUInt8 m_deviceStatus;

    //电压示数
    SdUInt32 m_comvol;

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
    SdUInt16 m_bk;

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
    SdUInt8 m_deviceStatus;          //0:stop work,1:start work
    SdUInt8 m_mqttStatus;            // 0:offline ,1:online,2:connected
    SdUInt8 m_pressValue_param;
    SdUInt16 m_check_acupointId;
    SdInt8 m_mqtt_client_isStart;
    SdInt8 m_device_collect;        //0:未采集，1:采集（根据血氧数据来判断）

}DeviceObjectDef,*LPDeviceObjectDef;


typedef struct TagPreCtrFrameDef   //心跳包消息队列,uart发送消息队列
{
    SdUInt16 msgID;
    SdUInt8 m_msgType;           //信息的下行或上传
    SdUInt16 m_deviceId; //[DEVICE_LENGTH1]
    
    SdUInt8 m_deviceType;        //设备类型
    
    //SdUInt8 m_123;           //信息的下行或上传
    
    SdUInt8 m_cmdType;
    SdUInt8 m_pressureid;
    SdUInt8 m_Ack;//命令类型
    

    //SdUInt8 m_deviceStatus;    //设备工作状态

}PreCtrFrameDef,*LPPreCtrFrameDef;

typedef struct TagPreCtrRecvFrameDef   //发送消息队列,uart发送消息队列
{
    SdUInt16 msgID;
    SdUInt8 m_msgType;           //信息的下行或上传
    SdUInt16 m_deviceId; //[DEVICE_LENGTH1]

    SdUInt8 m_deviceType;        //设备类型

    SdUInt8 m_cmdType;
    SdUInt8 m_pressureid;
    SdUInt8 m_stopFlag;
    //SdUInt8 m_Ack;//命令类型


}PreCtrRecvFrameDef,*LPPreCtrRecvFrameDef;



typedef enum TagPrectrDevList { 
    PressureSensor1, 
    PressureSensor2,
    PressureSensor3, 
    PressureSensor4, 
    PressureSensor5
}prectrDevList;    //设定一个设备id的数组

LPDeviceObjectDef device_ctrl_object_get(void);

/*STM32 uid*/

uint32_t get_STM32_uid(SdUInt16 * deviceid);
void get_esp8266_mac(char *macAddr);
void set_esp8266_mac(rt_uint32_t mac0,rt_uint32_t mac1,rt_uint32_t mac2);
SdUInt8 calcCRC(unsigned char *data,unsigned int len);

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
