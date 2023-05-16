/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */
#ifndef UT_UT_MQ_H_
#define UT_UT_MQ_H_

#include "ut_type.h"


#define UT_MQUEUE_MINMSG_COUNT  (10) //队列最大的消息数量
#define UT_MQUEUE_MAXMSG_COUNT  (100)
#define UT_MQUEUE_MSGMAX_SIZE  (512)  //队列消息最大长度，单位字节


/**
 * @brief  create mqueue
 * 
 * @param iMqueue OUTPUT
 * @param iName INPUT
 * @param iMsgSize INPUT
 * @param iMaxMsgs INPUT
 * @param iFlag INPUT
 * @return 
 */
UtMqueue * ut_mqueue_create(SdChar* iName,SdULong iMsgSize, SdULong iMaxMsgs, SdUInt8 iFlag);


/**
 * @brief  recv 
 * 
 * @param iMqueue INPUT
 * @param iBuffer INPUT
 * @param iSize INPUT
 * @param iTimeOut INPUT
 * @return 
 */
SdInt ut_mqueue_recv(UtMqueue *iMqueue,void* iBuffer,SdULong iSize, SdInt iTimeOut);


/**
 * @brief  send 
 * 
 * @param iMqueue INPUT
 * @param iBuffer INPUT
 * @param iSize INPUT
 * @return 
 */
SdInt ut_mqueue_send(UtMqueue *iMqueue,void* iBuffer,SdULong iSize);


/**
 * \struct  UTMsgDef
 * \brief   
 */
typedef struct tagUTMsgStr
{
    SdUShort    usSrcMod;       /**<  */
    SdUShort    usDstMod;       /**<  */
    SdUShort    usMsgID;        /**<  */
    SdUShort    usReserve;      /**<  */
    SdULong     ulContentLength;/**< */
}UTMsgDef, *LPUTMsgDef;

SdInt ut_msg_send(UtMqueue *iMqueue,
                    SdUShort usSrcModule, 
                    SdUShort usDstModule,
                    SdUShort usMsgID, 
                    void* pContent,SdULong ulContentLength);

SdInt ut_msg_recv(UtMqueue *iMqueue);


SdInt ut_mqueue_delete(UtMqueue *iMqueue);


#endif /* UT_UT_MQ_H_ */
