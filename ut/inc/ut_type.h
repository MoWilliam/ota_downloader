/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-11     Administrator       the first version
 */
#ifndef UT_INC_UT_TYPE_H_
#define UT_INC_UT_TYPE_H_

#include <rtthread.h> 

typedef int             SdBool;
typedef char            SdChar;
typedef unsigned char   SdUChar;
typedef short           SdShort;
typedef unsigned short  SdUShort;
typedef short           SdWord;
typedef int             SdInt;
typedef unsigned int    SdUInt;
typedef long            SdLong;
typedef unsigned long   SdULong;
typedef float           SdFloat;

typedef char            SdInt8;
typedef unsigned char   SdUInt8;
typedef short           SdInt16;
typedef unsigned short  SdUInt16;
typedef int             SdInt32;
typedef unsigned int    SdUInt32;

#define  SD_NULL        (0)
#define  SD_FALSE       (0)
#define  SD_TRUE        (1)
#define  SD_SUCCESS     (0)
#define  SD_FAIL        (-1)

typedef struct rt_thread UtThread;

typedef struct rt_messagequeue UtMqueue;
typedef struct rt_mailbox UtMessage;


#endif /* UT_INC_UT_TYPE_H_ */
