/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-13     Administrator       the first version
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>

#define DBG_ENABLE
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "paho_mqtt.h"
#include "inc/m_mqtt.h"
#include "inc/m_comm.h"

#define MQ_URI                "tcp://192.168.8.121:1883"
#define MQ_CLIENT             "f407vet6_client"
#define MQ_USERNAME           "admin"
#define MQ_PASSWORD           "admin"

//断掉之后发的包
#define MQ_SUBTOPIC           "/mqtt/f407vet6"
#define MQ_PUBTOPIC           "/mqtt/f407vet6"
#define MQ_WILLMSG            "Goodbye!"

/* define MQTT client context */
static MQTTClient mq_client;
static int mq_is_started = 0;

static void mq_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    char strTopic[256];
    rt_memset( strTopic,0,sizeof(strTopic));
    rt_strncpy( strTopic,(char *)msg_data->topicName->lenstring.data,msg_data->topicName->lenstring.len);
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    /*
    LOG_D("mqtt sub callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);*/
    comm_mqtt_subMsg(strTopic,(char *)msg_data->message->payload);
}

static void mq_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

static void mq_new_sub_callback(MQTTClient *client, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt new subscribe callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

static void mq_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
    comm_mqtt_status(emMqttConnectOk);
}

static void mq_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
    comm_mqtt_status(emMqttConnectOnline);
}

static void mq_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
    comm_mqtt_status(emMqttConnectOffline);
}

static void mq_pipe_callback(MQTTClient *c)
{
    //LOG_D("inter mqtt_pipe_callback!");
}

int mq_start(void)
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };

    if (mq_is_started)
    {
        LOG_E("mqtt client is already connected.");
        return -1;
    }
    /* config MQTT context param */
    {
        mq_client.isconnected = 0;
        mq_client.uri = MQ_URI;

        LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
        /* generate the random client ID */
        if ( pstDeviceObject){
            rt_snprintf(cid, sizeof(cid), "client-%s", pstDeviceObject->m_deviceId);
        }else{
            rt_strcpy(cid, MQ_CLIENT);
        }
        rt_kprintf("[MQTT Module]-> mqtt client run -> %s\n",cid);
        /* config connect param */
        memcpy(&mq_client.condata, &condata, sizeof(condata));
        mq_client.condata.clientID.cstring = cid;
        mq_client.condata.keepAliveInterval = 30;
        mq_client.condata.cleansession = 1;
        mq_client.condata.username.cstring = MQ_USERNAME;
        mq_client.condata.password.cstring = MQ_PASSWORD;

        /* config MQTT will param. */
        mq_client.condata.willFlag = 1;
        mq_client.condata.will.qos = 1;
        mq_client.condata.will.retained = 0;
        mq_client.condata.will.topicName.cstring = MQ_PUBTOPIC;
        mq_client.condata.will.message.cstring = MQ_WILLMSG;

        /* malloc buffer. */
        mq_client.buf_size = mq_client.readbuf_size = 1024;
        mq_client.buf = rt_calloc(1, mq_client.buf_size);
        mq_client.readbuf = rt_calloc(1, mq_client.readbuf_size);
        if (!(mq_client.buf && mq_client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            return -1;
        }

        /* set event callback function */
        mq_client.connect_callback = mq_connect_callback;
        mq_client.online_callback = mq_online_callback;
        mq_client.offline_callback = mq_offline_callback;
        mq_client.pipe_callback = mq_pipe_callback;

        /* set subscribe table and event callback */
        mq_client.messageHandlers[0].topicFilter = rt_strdup(MQ_CONTROL_TOPIC);
        mq_client.messageHandlers[0].callback = mq_sub_callback;
        mq_client.messageHandlers[0].qos = QOS1;

        /* set default subscribe event callback */
        mq_client.defaultMessageHandler = mq_sub_default_callback;
    }

    /* run mqtt client */
    paho_mqtt_start(&mq_client);
    mq_is_started = 1;

    return 0;
}

int mq_stop(void)
{
    mq_is_started = 0;

    return paho_mqtt_stop(&mq_client);
}

int mq_publish(char *topic ,char *message)
{
    //return 1;
    int ret =0;
    if (mq_is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    if (topic != NULL && message!= NULL)
    {
        ret = paho_mqtt_publish(&mq_client, QOS1, topic, message);
    }
    else
    {
        rt_kprintf("mqtt_publish <topic> [message]  --mqtt publish message to specified topic.\n");
        return -1;
    }

    return ret;
}

int mq_subscribe(char *topic)
{
    if (mq_is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_subscribe(&mq_client, QOS1, topic, mq_new_sub_callback);
}

int mq_unsubscribe(char *topic)
{
    if (mq_is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_unsubscribe(&mq_client, topic);
}
