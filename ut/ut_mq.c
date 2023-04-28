/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */

#include "inc/ut_mq.h"
#include "application.h"
 
UtMqueue * ut_mqueue_create(SdChar* iName,
                       SdULong iMsgSize, SdULong iMaxMsgs, SdUInt8 iFlag)
{
    struct rt_messagequeue *iMqueue = rt_mq_create(iName,iMsgSize,iMaxMsgs,iFlag);
    if ( iMqueue == RT_NULL)
    {
        rt_kprintf("[UMqueue] %s UMqueue Create Error\n",iName);
        return RT_NULL;
    }
    rt_kprintf("[UMqueue] %s UMqueue Create Ok\n",iName);
    return iMqueue;
}


SdInt ut_mqueue_send(UtMqueue *iMqueue,void* iBuffer,SdULong iSize)
{
    if (RT_NULL != iMqueue)
    {
        rt_err_t rt_ret =  rt_mq_send(iMqueue,iBuffer,iSize);
        if (-RT_EFULL == rt_ret)
        {
            rt_kprintf("[UMqueue] Queue is Full Code[%d]\n",rt_ret);
        }
        if (-RT_ERROR == rt_ret)
        {
            rt_kprintf("[UMqueue] Data Size Overflow Code[%d]\n",rt_ret);
        }
        if (RT_EOK != rt_ret)
        {
            rt_kprintf("[UMqueue] Send Buffer Error Code[%d]\n",rt_ret);
            return -1;
        }
    }
    else
    {
        rt_kprintf("[UMqueue] Send Buffer Error iMqueue is Null\n");
        return -1;
    }
    rt_kprintf("[UMqueue] Send Buffer Ok[%d]\n",iSize);
    return 0;
}

SdInt ut_mqueue_recv(UtMqueue *iMqueue,void* iBuffer,SdULong iSize, SdInt iTimeOut)
{
    if (RT_NULL != iMqueue)
    {
        rt_err_t rt_ret =  rt_mq_recv(iMqueue,iBuffer,iSize,iTimeOut);
        if (-RT_ETIMEOUT == rt_ret)
        {
            rt_kprintf("[UMqueue] Recv Data TimeOut !\n");
        }
        if (RT_EOK != rt_ret )
        {
            rt_kprintf("[UMqueue] Recv Data Error Code[%d]\n" ,rt_ret);
            return -1;
        }
    }
    else
    {
        rt_kprintf("[UMqueue] Recv Data Error iMqueue is Null\n");
        return -1;
    }
    rt_kprintf("[UMqueue] Recv Data Ok[%d]\n",iSize);
    return 0;
}


SdInt ut_msg_send(UtMqueue *iMqueue, SdUShort usSrcModule,SdUShort usDstModule,SdUShort usMsgID, void* pContent,SdULong ulContentLength)
{
    UTMsgDef stshMsg;
    SdChar * buffer = SD_NULL;
    SdChar * ptmp = SD_NULL;
    SdInt nlen = 0;
    SdInt nRn = 0;     
    stshMsg.usSrcMod = usSrcModule;
    stshMsg.usDstMod = usDstModule;
    stshMsg.usMsgID = usMsgID;
    stshMsg.ulContentLength = ulContentLength;
    stshMsg.usReserve = 0;

    nlen = sizeof(UTMsgDef) + ulContentLength;
    buffer = rt_malloc(nlen);
    rt_memcpy(buffer, &stshMsg, sizeof(UTMsgDef));
    ptmp = buffer + sizeof(UTMsgDef);
    if(ulContentLength > 0 && SD_NULL != pContent)
    {
        rt_memcpy(ptmp, pContent, ulContentLength);
    }
    rt_err_t rt_ret = rt_mq_send(iMqueue,buffer,nlen);
    if (-RT_EFULL == rt_ret)
    {
        rt_kprintf("[UTMsg %d,%d]send msg %d fail,Queue is Full !\n",usSrcModule,usDstModule,usMsgID);
    }
    if (-RT_ERROR == rt_ret)
    {
        rt_kprintf("[UTMsg %d,%d]send msg %d fail! Data Size Overflow\n",usSrcModule,usDstModule,usMsgID);
    }
    if (RT_EOK == rt_ret)
    {
        rt_kprintf("[UTMsg %d,%d]send msg %d successful\n",usSrcModule,usDstModule,usMsgID);
    }

    rt_free((void*)buffer);
    return nRn;
}

SdInt ut_msg_recv(UtMqueue *iMqueue)
{
    UTMsgDef stMsg;
    SdInt MsgLen = sizeof(UTMsgDef);
    void *pContent = SD_NULL;
    SdChar* msg = SD_NULL;
    msg = rt_malloc(UT_MQUEUE_MSGMAX_SIZE);
    rt_kprintf("[UTMsg] Start Recv Data !\n");
    rt_err_t rt_ret = rt_mq_recv(iMqueue,msg,UT_MQUEUE_MSGMAX_SIZE,RT_WAITING_FOREVER);
    if (-RT_ETIMEOUT == rt_ret)
    {
        rt_kprintf("[UTMsg] Recv Data TimeOut !\n");
    }
    if (RT_EOK != rt_ret )
    {
        rt_kprintf("[UTMsg] Recv Data Error Code[%d]\n" ,rt_ret);
    }
    if (RT_EOK == rt_ret )
    {
        rt_kprintf("[UTMsg] queue revc successful: %s\n",msg);
        rt_memcpy(&stMsg, msg, MsgLen);
        if (stMsg.ulContentLength > 0)
        {
            pContent = msg + MsgLen;
        }
        else if(stMsg.ulContentLength <= 0)
        {
            pContent = SD_NULL;
        }
        app_msg_handle(&stMsg,pContent);
    }

    if (msg != SD_NULL)
    {
        rt_free(msg);
        msg = SD_NULL;
    }
    return 0;
}

SdInt ut_mqueue_delete(UtMqueue *iMqueue)
{
    rt_mq_delete(iMqueue);
    return 0;
}