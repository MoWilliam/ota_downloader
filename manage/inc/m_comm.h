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

//命令类型定义0：心跳命令；1：启动测量；2：结束测量；3：参数设置；4：数据上传；5：开始充气；6：停止充气
typedef enum TagMqttCmdDef
{
    emMqttCmdUnknown = -1,
    emMqttCmdHeartBeat,
    emMqttCmdStart,
    emMqttCmdStop,
    emMqttCmdSetting,
    emMqttCmdData,
    emMqttCmdStartPress,
    emMqttCmdStopPress

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



