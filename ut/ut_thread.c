/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */

#include "inc/ut_thread.h"

SdInt ut_thread_create(UtThread *iThread,SdChar* iName,
                       SdUInt32 iStackSize, SdUInt8 iPriority, SdUInt32 iTick,
                       void (*func)(void *), void *arg)
{

    iThread = rt_thread_create(iName,func,arg,iStackSize,iPriority,iTick);
    if ( iThread == RT_NULL)
    {
        rt_kprintf("[UThread] %s Thread Create Error Code\n",iName);
        return -1;
    }
    rt_kprintf("[UThread] %s Thread Create Ok\n",iName);
    ut_thread_startup(iThread);
    return 0;
}


SdInt ut_thread_init(UtThread *iThread,SdChar* iName,
                       SdUInt8 *iStackStart, SdUInt iStatckSize, 
                       SdUInt8 iPriority, SdUInt32 iTick,
                       void (*func)(void *), void *arg)
{
    rt_kprintf("[UThread] Thread Init iStatckSize[%d]\n" ,iStatckSize);
    rt_err_t rt_ret = rt_thread_init(iThread,iName,func,arg,iStackStart,iStatckSize,iPriority,iTick);
    if ( RT_EOK != rt_ret)
    {
        rt_kprintf("[UThread] %s Thread Init Error Code[%d]\n",iName ,rt_ret);
        return -1;  
    }
    rt_kprintf("[UThread] %s Thread Init Ok\n",iName);
    ut_thread_startup(iThread);
    return 0;
}


SdInt ut_thread_startup(UtThread *iThread)
{
    if ( RT_NULL != iThread)
    {
        rt_err_t rt_ret =  rt_thread_startup(iThread);
        if (RT_EOK != rt_ret )
        {
            rt_kprintf("[UThread] Thread StartUp Error Code[%d]\n" ,rt_ret);
            return -1;
        }
    }
    rt_kprintf("[UThread] Thread startup Ok\n");
    return 0;

}

void ut_thread_exit(UtThread *iThread)
{
    rt_thread_delete(iThread);
}