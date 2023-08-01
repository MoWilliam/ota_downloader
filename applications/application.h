/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */
#ifndef APPLICATIONS_APPLICATION_H_
#define APPLICATIONS_APPLICATION_H_

#include "ut/inc/ut_type.h"
#include "ut/inc/ut_message.h"
#include "ut/inc/ut_mq.h"
#include "ut/inc/ut_thread.h"
#include "manage/inc/m_device.h"

#define COMPOSITE_CONTROL_FLAG 1 // 综合传感器，
#define PRESS_CONTROL_FLAG 0  // 压力控制器

/** 
 * Application object
 * thread
 * data
 * 
 */
typedef struct TagAppObjectDef
{
    UtThread*  Appthread_msg;
    SdBool    brun;
    UtThread*  MqttThead_heartBeat;
    SdBool    brun_mqtt;

}AppObjectDef,*LPAppObjectDef;


/** 
 * Task module object
 * thread
 * data
 * 
 */
typedef struct TagTaskObjectDef
{
    UtThread*  Taskthread_temp;
    SdBool     brun;
    UtThread*  Taskthread_bio;
    SdBool     brun_bio;
    UtThread*  Taskthread_spo2;
    SdBool     brun_spo2;
    UtThread*  Taskthread_power;
    SdBool     brun_sensor;

}TaskObjectDef,*LPTaskObjectDef;

/** 
 * Manage module object
 * thread
 * data
 * 
 */
typedef struct TagManageObjectDef
{
    UtThread*  Managethread_data;
    SdBool     brun_data;

}ManageObjectDef,*LPManageObjectDef;


/** 
 * PressControl object
 * thread
 * data
 * 
 */
typedef struct TagPressControlObjectDef
{
    UtThread*  MqThead_prectrheartBeat;   //心跳包线程以及标志位
    SdBool    brun_mqprectrheartBeat;
    UtThread*  Thead_prectrRecv;   //加压和减压以及标志位
    SdBool    brun_prectrRecv;
    UtThread*  Thead_prectrUart;   //加压和减压以及标志位
    SdBool    brun_prectrUart;

}PressControlObjectDef,*LPPressControlObjectDef;

/** 
 * UtMqueue object
 * 
 */
typedef struct TagMqueueObjectDef
{
    UtMqueue*  MMqueue_sensor;
    SdChar *   MMqueue_sensor_name;
    UtMqueue*  MMqueue_bio;
    SdChar *   MMqueue_bio_name;
    UtMqueue*  MMqueue_msg;
    SdChar *   MMqueue_msg_name;
    UtMqueue*  MMqueue_preheartbeat;
    SdChar *   MMqueue_preheartbeat_name;

}MqueueObjectDef,*LPMqueueObjectDef;


/** 
 * UtMessage object
 * 
 */
typedef struct TagMessageObjectDef
{
    UtMessage* MMessage_data;
    SdChar * MMessage_data_name;

}MessageObjectDef,*LPMessageObjectDef;


/**
* \enum  enMqttMsgDef
* \brief running-state
*/
typedef enum TagObjectMsgDef
{
    emObjectMsgUnknown,
    emMqttMsgBaseData,
    emMqttMsgBioData,
    emMqttMsgSpo2Data

}enObjectMsgDef;

/** 
 * get app module object
 * 
 */
LPAppObjectDef app_ctrl_object_get(void);

/** 
 * get task module object
 * 
 */
LPTaskObjectDef task_ctrl_object_get(void);

/** 
 * get Manage module object
 * 
 */
LPManageObjectDef manage_ctrl_object_get(void);

/** 
 * get mqueue object
 * 
 */
LPMqueueObjectDef mq_ctrl_object_get(void);

/** 
 * get message object
 * 
 */
LPMessageObjectDef message_ctrl_object_get(void);

/** 
 * get PressControl module object
 * 
 */
LPPressControlObjectDef pressControl_ctrl_object_get(void);


/**
 * @brief  handle all message of app
 * 
 * @param pMsg OUTPUT message type
 * @param pContent OUTPUT message value
 * @return 
 */
SdInt app_msg_handle(const UTMsgDef * pMsg, const void * pContent);


void appStart(void);

void appStop(void);

#endif /* APPLICATIONS_APPLICATION_H_ */