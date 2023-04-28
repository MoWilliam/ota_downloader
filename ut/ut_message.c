/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */
#include "inc/ut_message.h"

UtMessage * ut_message_create(SdChar* iName,
                       SdULong iMaxMsgs, SdUInt8 iFlag)
{
    struct ut_message * iMessage = rt_mb_create(iName,iMaxMsgs,iFlag);
    if ( iMessage == RT_NULL)
    {
        rt_kprintf("[UMessage] UMessage Create Error\n");
        return RT_NULL;
    }
    rt_kprintf("[UMessage] UMessage Create Ok\n");
    return iMessage;
}

SdInt ut_message_send(UtMessage *iMessage,SdULong iMegValue)
{
    if (RT_NULL != iMessage)
    {
        rt_err_t rt_ret =  rt_mb_send(iMessage,iMegValue);
        if (RT_EOK != rt_ret)
        {
            rt_kprintf("[UMessage] Send Msg Error Code[%d]\n",rt_ret);
            return -1;
        }
    }
    else
    {
        rt_kprintf("[UMessage] Send Msg Error iMessage is Null\n");
        return -1;
    }
    rt_kprintf("[UMessage] Send Msg Ok[%d]\n",iMegValue);
    return 0;
}

SdInt ut_message_recv(UtMessage *iMessage,SdULong* iMegValue,SdInt iTimeOut)
{
    if (RT_NULL != iMessage)
    {
        rt_err_t rt_ret =  rt_mb_recv(iMessage,iMegValue,iTimeOut);
        if (-RT_ETIMEOUT == rt_ret)
        {
            rt_kprintf("[UMessage] Recv Msg TimeOut !\n");
        }
        if (RT_EOK != rt_ret )
        {
            rt_kprintf("[UMessage] Recv Msg Error Code[%d]\n" ,rt_ret);
            return -1;
        }
    }
    else
    {
        rt_kprintf("[UMessage] Recv Msg Error iMessage is Null\n");
        return -1;
    }
    rt_kprintf("[UMessage] Recv Msg Ok[%d]\n", *iMegValue);
    return 0;
}

SdInt ut_message_delete(UtMessage *iMessage)
{
    rt_mb_delete(iMessage);
    return 0;
}