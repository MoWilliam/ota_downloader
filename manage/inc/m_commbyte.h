/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-30     Administrator       the first version
 */

#ifndef MANAGE_INC_M_COMMBYTE_H_
#define MANAGE_INC_M_COMMBYTE_H_

#include "application.h"
#include "bsp/inc/b_prectrgpio.h"

#define MQ_HEARTBEAT_TOPIC          "/medical/heartbeat"
#define MQ_CONTROL_TOPIC            "/medical/control"
#define PreCtr_UART_NAME            "uart4"
#define UART_BUFFER_SIZE            256

/*尝试字节流的编写*/
//命令类型定义0：心跳命令；1：参数设置；2：开始加压；3：泄压操作。


typedef enum TagUartCmdDef
{
    emUartCmdUnknown = -1,
    emUartCmdHeartBeat,   //气动单元的设备
    emUartCmdStartPress,
    emUartCmdStopPress,
    emUartCmdHoldPress,
    emUartCmdSetting,
    emUartCmdDataup,
    emUartCmdFirmwareVersion,


}enUartCmdDef;

typedef enum  
{
    emUartMsgTypeUnknown = -1,
    emUartMsgTypeUp,
    emUartMsgTypeDown

}enUartMsgTypeDef;

typedef enum TagUartConnectStatusDef
{
    emUartConnectStatusUnknown,
    emUartConnectOk,
    emUartConnectOnline,
    emUartConnectOffline
}enUartConnectStatusDef;




void manage_commbyte_init(void);
void manage_commbyte_start(void);
void manage_commbyte_stop(void);

// void manage_commbytecmd_init(void);
// void manage_commbytecmd_start(void);
// void manage_commbytecmd_stop(void);

#endif /* MANAGE_INC_M_COMMBYTE_H_ */
