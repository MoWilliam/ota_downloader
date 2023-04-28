/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */
#ifndef UT_UT_MESSAGE_H_
#define UT_UT_MESSAGE_H_

#include "ut_type.h"


#define UT_MESSAGE_MAXMSG_SIZE  (1024)
         
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
UtMessage *  ut_message_create(SdChar* iName,
                       SdULong iMaxMsgs, SdUInt8 iFlag);

/**
 * @brief  send
 * 
 * @param iMqueue INPUT
 * @param iMegValue INPUT
 * @return 
 */
SdInt ut_message_send(UtMessage *iMessage,SdULong iMegValue);


/**
 * @brief  recv
 * 
 * @param iMqueue INPUT
 * @param iMegValue INPUT
 * @param iTimeOut INPUT
 * @return 
 */
SdInt ut_message_recv(UtMessage *iMessage,SdULong* iMegValue,SdInt iTimeOut);

SdInt ut_message_delete(UtMessage *iMessage);

#endif /* UT_UT_MESSAGE_H_ */
