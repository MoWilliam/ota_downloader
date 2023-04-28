/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */
#ifndef UT_UT_THREAD_H_
#define UT_UT_THREAD_H_

#include "ut_type.h"

#define UT_THREAD_PRIORITY_DEFAULT  (25)
#define UT_THREAD_PRIORITY_HIGH     (0)
#define UT_THREAD_PRIORITY_LOW      (32)

#define UT_THREAD_STACK_SIZE_DEFAULT    (512)
#define UT_THREAD_STACK_SIZE_MID        (1024)
#define UT_THREAD_STACK_SIZE_LARGE      (2048)

#define UT_THREAD_TICK_DEFAULT (10)
#define UT_THREAD_TICK_LARGE   (50)

#define UT_THREAD_MSDELAY(ms) rt_thread_mdelay(ms)
#define UT_THREAD_DELAY(s) rt_thread_delay(s)


/**
 * @brief 
 * 
 * @param iThread OUTPUT
 * @param iName INPUT
 * @param iStackSize INPUT
 * @param iPriority INPUT
 * @param iTick INPUT
 * @param func INPUT CALLBACK FUNCTION
 * @param arg INPUT
 * @return 
 */
SdInt ut_thread_create(UtThread *iThread,SdChar* iName,
                       SdUInt32 iStackSize, SdUInt8 iPriority, SdUInt32 iTick,
                       void (*func)(void *), void *arg);


/**
 * @brief 
 * 
 * @param iThread OUTPUT
 * @param iName INPUT
 * @param iStackSize INPUT
 * @param iPriority INPUT
 * @param iTick INPUT
 * @param func INPUT CALLBACK FUNCTION
 * @param arg INPUT
 * @return 
 */
SdInt ut_thread_init(UtThread *iThread,SdChar* iName,
                       SdUInt8 *iStackStart, SdUInt iStatckSize, 
                       SdUInt8 iPriority, SdUInt32 iTick,
                       void (*func)(void *), void *arg);


/**
 * @brief 
 * 
 * @param iThread INPUT
 * @return 
 */
SdInt ut_thread_startup(UtThread *iThread);


/**
 * @brief 
 * 
 * @param iThread INPUT
 * @return 
 */
void ut_thread_exit(UtThread *iThread);


#endif /* UT_UT_THREAD_H_ */
