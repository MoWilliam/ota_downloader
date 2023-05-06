/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
#include "application.h"

#define MQ_HEARTBEAT_TOPIC          "/medical/heartbeat"
#define MQ_CONTROL_TOPIC            "/medical/control"
#define MQ_DATA_TOPIC               "/medical/data"

typedef enum TagMqttCmdDef
{
    emMqttCmdUnknown,
    emMqttCmdHeartBeat,
    emMqttCmdStart,
    emMqttCmdStop,
    emMqttCmdSetting,
    emMqttCmdData

}enMqttCmdDef;

typedef enum TagMqttMsgTypeDef
{
    emMqttMsgTypeUnknown,
    emMqttMsgTypeUp,
    emMqttMsgTypeDown

}enMqttMsgTypeDef;

typedef enum TagMqttConnectStatusDef
{
    emMqttConnectStatusUnknown,
    emMqttConnectOk,
    emMqttConnectOnline,
    emMqttConnectOffline

}enMqttConnectStatusDef;

void manage_comm_init(void);
void manage_comm_start(void);
void manage_comm_stop(void);

SdInt comm_mqtt_msg(const UTMsgDef *pMsg, const void *pContent);
SdInt comm_mqtt_subMsg(char* topic ,char * message);
SdInt comm_mqtt_status(int connectStatus);



