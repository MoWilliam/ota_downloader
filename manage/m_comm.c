/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
#include "bsp/inc/b_adc.h"
#include "inc/m_comm.h"
#include "inc/m_mqtt.h"
#include "ut/inc/cJSON.h"
#include "inc/m_device.h"


static SdULong g_msgId_hearBeat;
static SdULong g_msgId_control;
static SdULong g_msgId_data;

SensorDataFrameDef g_mqttDmf;
DataFrameDef g_temp_mqttDmf;
Spo2FrameDef g_spo2_mqttDmf;
BioFrameDef g_bio_mqttDmf;


//void sensorDataToJSON(SensorDataFrameDef *dmf, cJSON *root_json);
void heartbeatToJSON();

void mqtt_thread_heartbeat(void *ptr)
{
    rt_kprintf("mqtt_thread_heartbeat thread run\n");
    if(SD_NULL != ptr)
    {
        LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
        LPAppObjectDef pstAppObject = (LPAppObjectDef)ptr;
        while (pstAppObject->brun)
        {
            if ( pstDeviceObject)
            {
                if( pstDeviceObject->m_mqtt_client_isStart ==0)
                {
                    // 启动MQTT
                    if ( strlen(pstDeviceObject->m_deviceId)>0)
                    {
                        mq_start();
                        pstDeviceObject->m_mqtt_client_isStart = 1;
                    }
                    rt_thread_mdelay(1000);
                }
                else
                {
                    // 维持不同状态下的心跳
                    if ( pstDeviceObject->m_mqttStatus == 0 || pstDeviceObject->m_deviceStatus == 0)
                    {
                        heartbeatToJSON();
                        rt_kprintf("[MQTT Module]-> heartbeat thread run\n");
                    }
                    rt_thread_mdelay(1000*5);
                }
            }
        }
        rt_kprintf("[MQTT Module] thread exit\n");
        ut_thread_exit(pstAppObject->MqttThead_heartBeat);
    }
}

void comm_mqtt_heartBeat(void)
{
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    if(SD_NULL != pstAppObject)
    {
        if ( pstAppObject->brun_mqtt == SD_FALSE)
        {
            pstAppObject->brun_mqtt = SD_TRUE;
            ut_thread_create(pstAppObject->MqttThead_heartBeat,"MQTT_HEARTBEAT_THREAD",
                            UT_THREAD_STACK_SIZE_LARGE,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_DEFAULT,
                            mqtt_thread_heartbeat,pstAppObject);
        }

    }
}

SdInt comm_mqtt_status(int connectStatus)
{
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
    switch (connectStatus) {
        case emMqttConnectOk:
            {
                if ( pstDeviceObject)
                {
                    pstDeviceObject->m_mqttStatus = 2;
                    rt_kprintf("MQTT SERVIER STATUS CONNECTED\n");
                }
            }
            break;
        case emMqttConnectOnline:
            {
                if ( pstDeviceObject)
                {
                    pstDeviceObject->m_mqttStatus = 1;
                    rt_kprintf("MQTT SERVIER STATUS ONLINE\n");
                }
            }
            break;

        case emMqttConnectOffline:
            {
                if ( pstDeviceObject)
                {
                    pstDeviceObject->m_mqttStatus = 0;
                    rt_kprintf("MQTT SERVIER STATUS OFFLINE\n");
                }
            }
            break;

        default:
            break;
    }
    return 0;
}

SdInt comm_mqtt_subMsg(char* topic, char * message)    //订阅
{
    rt_kprintf("MQTT MSG->topic:%s,msg:%s\n",topic,message);
    cJSON *root = NULL,*data = NULL;
    cJSON *msgType = NULL, *deviceId = NULL, *deviceType = NULL,*cmdType = NULL;
    cJSON *acupointId = NULL,*pressure = NULL;

    if ( rt_strcmp( topic,MQ_CONTROL_TOPIC) == 0)  //控制主题的消息
    {
        root = cJSON_Parse(message);  // 用于解析 JSON 字符串并构建一个 cJSON 数据结构以便于后续的 JSON 数据处理。
        if(root)
        {
            msgType = cJSON_GetObjectItem(root, "msgType");     // 用于从 JSON 对象中获取指定名称的元素（字段）的指针
            deviceId = cJSON_GetObjectItem(root, "deviceId");
            deviceType = cJSON_GetObjectItem(root, "deviceType");
            cmdType = cJSON_GetObjectItem(root, "cmdType");
            data = cJSON_GetObjectItem(root, "data");
            LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
            if ( msgType && deviceId && deviceType && cmdType)
            {
                if ( msgType->valueint == emMqttMsgTypeDown)
                {
                    acupointId = cJSON_GetObjectItem(data, "acupointId");
                    pressure = cJSON_GetObjectItem(data, "pressure");
                    if ( deviceType->valueint == emDeviceCompositeSensor)
                    {
                        if ( rt_strcmp(deviceId->valuestring, pstDeviceObject->m_deviceId) == 0)
                        {
                            if ( cmdType->valueint == emMqttCmdStart){
                                pstDeviceObject->m_deviceStatus = 1;
                            }
                            else if ( cmdType->valueint == emMqttCmdStop){
                                pstDeviceObject->m_deviceStatus = 0;
                            }
                            if ( acupointId){
                                pstDeviceObject->m_check_acupointId = atoi(acupointId->valuestring);
                            }
                        }
                    }

                    if ( deviceType->valueint == emDevicePressSensor)
                    {
                        if ( acupointId){
                            pstDeviceObject->m_check_acupointId = atoi(acupointId->valuestring);        //atoi 是一个标准 C 库函数，用于将字符串转换为整数。\
                                                                                                        它会从字符串中解析整数值，直到遇到非数字字符为止。如\
                                                                                                     果字符串无法解析为有效整数，将返回 0。
                        }

                        if ( pressure){
                            pstDeviceObject->m_pressValue_param = atoi(pressure->valuestring);
                        }

                    }

                    if ( deviceType->valueint == emDevicePressControlSensor)      //气动单元部分
                    {
                        if ( cmdType->valueint == emMqttCmdStart){
                            pstDeviceObject->m_deviceStatus = 1;
                        }
                        else if ( cmdType->valueint == emMqttCmdStop){
                            pstDeviceObject->m_deviceStatus = 0;
                        }
                    }
                }
            }
        }
        cJSON_Delete(root);  //释放，以免造成指针泄漏
    }

    return 0;
}


SdInt comm_mqtt_msg(const UTMsgDef *pMsg, const void *pContent)
{
   // return 0;
  //  rt_mutex_take(&mutex, RT_WAITING_FOREVER); // 锁定互斥锁
//    cJSON *root_json = NULL;
//    char *out = NULL;
//    int len = 0;
//    int ret = 0;
//
//    root_json = cJSON_CreateObject();

    if( pMsg->usMsgID == emMqttMsgBaseData)
    {
        if ( pContent != SD_NULL)
        {
            LPSensorDataFrameDef pdmf = (LPSensorDataFrameDef)pContent;
            if ( pdmf != NULL)
            {
                rt_kprintf("[Mqtt Module] emMqttMsgBaseData handle data %d ,%d\n",pdmf->m_atemp,pdmf->m_btemp);

                g_temp_mqttDmf.m_atemp = pdmf->m_atemp;
                g_temp_mqttDmf.m_btemp = pdmf->m_btemp;

//                DataFrameDef mqttDmf;
//                mqttDmf.m_atemp = pdmf->m_atemp;
//                mqttDmf.m_btemp = pdmf->m_btemp;

//                baseDataToJSON(&mqttDmf,root_json);
//                out = cJSON_PrintUnformatted(root_json);
//                len = strlen(out);
//                if ( len >0)
//                {
//                    ret = mq_publish(MQ_DATA_TOPIC, out);
//                    rt_kprintf("MQTT MSG->topic:%s,ret:%d len:%d msg:%s\n",MQ_DATA_TOPIC,ret,len, out);
//                }
//                cJSON_free(out);
            }

        }
    }
    else if ( pMsg->usMsgID == emMqttMsgSpo2Data)
    {
        if ( pContent != SD_NULL)
        {
            LPSensorDataFrameDef pdmf = (LPSensorDataFrameDef)pContent;
            if ( pdmf != NULL)
            {
                rt_kprintf("[Mqtt Module] emMqttMsgSpo2Data handle data %d\n",pdmf->m_spo2);

                g_spo2_mqttDmf.m_spo2 = pdmf->m_spo2;
                g_spo2_mqttDmf.m_bk = pdmf->m_bk;
//                Spo2FrameDef mqttDmf;
//                mqttDmf.m_spo2 = pdmf->m_spo2;
//                mqttDmf.m_bk = pdmf->m_bk;
//                spo2DataToJSON(&mqttDmf,root_json);
//                out = cJSON_PrintUnformatted(root_json);
//                len = strlen(out);
//                if ( len >0)
//                {
//                    ret = mq_publish(MQ_DATA_TOPIC, out);
//                    rt_kprintf("MQTT MSG->topic:%s,ret:%d len:%d msg:%s\n",MQ_DATA_TOPIC,ret,len, out);
//                }
//                cJSON_free(out);
            }

        }
    }
    else if ( pMsg->usMsgID == emMqttMsgBioData)
    {
        if ( pContent != SD_NULL)
        {
            LPSensorDataFrameDef pbmf = (LPSensorDataFrameDef)pContent;
            if ( pbmf != NULL)
            {
                rt_kprintf("[Mqtt Module] emMqttMsgBioData handle data %d \n",pbmf->m_bio_value);

                g_bio_mqttDmf.m_bio_ampere = pbmf->m_bio_ampere;
                g_bio_mqttDmf.m_bio_voltage = pbmf->m_bio_voltage;
                g_bio_mqttDmf.m_bio_value = pbmf->m_bio_value;
//                BioFrameDef mqttDmf;
//                mqttDmf.m_bio_ampere = pbmf->m_bio_ampere;
//                mqttDmf.m_bio_voltage = pbmf->m_bio_voltage;
//                mqttDmf.m_bio_value = pbmf->m_bio_value;

//                bioDataToJSON(&mqttDmf,root_json);
//                out = cJSON_PrintUnformatted(root_json);
//                len = strlen(out);
//                if ( len >0)
//                {
//                    ret = mq_publish(MQ_DATA_TOPIC, out);
//                    rt_kprintf("MQTT MSG->topic:%s,ret:%d len:%d msg:%s\n",MQ_DATA_TOPIC,ret,len, out);
//                }
//                cJSON_free(out);
            }
        }
    }
  //  cJSON_Delete(root_json);
  //  rt_mutex_release(&mutex); // 解锁互斥锁
    return 0;
}  

void sensorDataToJSON(SensorDataFrameDef *dmf)   //传感器数据的整体打包
{
    char tValue_temp[8], tValue_spo2[8], tValue_bio[8];
    cJSON *root_json = cJSON_CreateObject();
    cJSON *data_json = NULL;
    cJSON *phase_array = NULL;
    char *out = NULL;
    int len = 0;
    int ret = 0;
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
    if (pstDeviceObject)
    {
        cJSON_AddNumberToObject(root_json, "msgId", g_msgId_data++);   //将一个数字值添加到 cJSON 对象中
        cJSON_AddNumberToObject(root_json, "msgType", emMqttMsgTypeUp);
        cJSON_AddStringToObject(root_json, "deviceId", pstDeviceObject->m_deviceId);
        cJSON_AddNumberToObject(root_json, "deviceType", emDeviceCompositeSensor);
        cJSON_AddNumberToObject(root_json, "cmdType", emMqttCmdData);
        cJSON_AddNumberToObject(root_json, "acupointId", pstDeviceObject->m_check_acupointId);

        phase_array = cJSON_CreateArray();
        
        // 温度数据
        data_json = cJSON_CreateObject();
        memset(tValue_temp, 0, 8);
        double temp_Value = dmf->m_btemp * 0.00390625;
        temp_Value += dmf->m_atemp;
        sprintf(tValue_temp, "%.2f", temp_Value);  //改变温度数值精度
        cJSON_AddStringToObject(data_json, "tempValue", tValue_temp);
        //cJSON_AddItemToArray(phase_array, data_json);

        // Spo2 数据
        //data_json = cJSON_CreateObject();
        memset(tValue_spo2, 0, 8);
        sprintf(tValue_spo2, "%d", dmf->m_spo2);
        cJSON_AddStringToObject(data_json, "spo2Value", tValue_spo2);
        memset(tValue_spo2, 0, 8);
        sprintf(tValue_spo2, "%d", dmf->m_bk);
        cJSON_AddStringToObject(data_json, "bkValue", tValue_spo2);
        //cJSON_AddItemToArray(phase_array, data_json);

        // 生物数据
        //data_json = cJSON_CreateObject();
        memset(tValue_bio, 0, 8);
        sprintf(tValue_bio, "%d", dmf->m_bio_value);
        cJSON_AddStringToObject(data_json, "bioValue", tValue_bio);

        //整体打包
        cJSON_AddItemToArray(phase_array, data_json);
        cJSON_AddItemToObject(root_json, "data", phase_array);

        out = cJSON_PrintUnformatted(root_json);
        len = strlen(out);
        if ( len >0)
        {
            ret = mq_publish(MQ_DATA_TOPIC, out);
            rt_kprintf("MQTT MSG->topic:%s,ret:%d len:%d msg:%s\n",MQ_DATA_TOPIC,ret,len, out);
        }
        cJSON_free(out);
    }
    cJSON_Delete(root_json);
} 

void mqtt_thread_sensordata(void *ptr)
{
    rt_kprintf("mqtt_thread_sensordata thread run\n");
    if(SD_NULL != ptr)
    {
        LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
        LPAppObjectDef pstAppObject = (LPAppObjectDef)ptr;
        while (pstAppObject->brun_sensor)
        {
            if ( pstDeviceObject)
            {
              //  rt_mutex_take(&mutex, RT_WAITING_FOREVER); // 锁定互斥锁
                SensorDataFrameDef dmf;
                dmf.m_atemp = g_temp_mqttDmf.m_atemp;
                dmf.m_btemp = g_temp_mqttDmf.m_btemp;

                dmf.m_spo2 = g_spo2_mqttDmf.m_spo2;
                dmf.m_bk = g_spo2_mqttDmf.m_bk;

                dmf.m_bio_value =  g_bio_mqttDmf.m_bio_value;

                if ( dmf.m_atemp >0 || dmf.m_spo2 >0 || dmf.m_bk >0 || dmf.m_bio_value >0){
                    sensorDataToJSON(&dmf);
                }

              //  rt_mutex_release(&mutex); // 解锁互斥锁

            }
            rt_thread_mdelay(1000);
        }
        rt_kprintf("[MQTT Module] thread exit\n");
        ut_thread_exit(pstAppObject->MqttThead_sensorData);
    }
}

void comm_mqtt_sensondata(void)
{
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    if(SD_NULL != pstAppObject)
    {
        if ( pstAppObject->brun_sensor == SD_FALSE)
        {
            pstAppObject->brun_sensor = SD_TRUE;
            ut_thread_create(pstAppObject->MqttThead_sensorData,"MQTT_SENSORDATA_THREAD",
                            UT_THREAD_STACK_SIZE_LARGE,
                            UT_THREAD_PRIORITY_DEFAULT,
                            UT_THREAD_TICK_DEFAULT,
                            mqtt_thread_sensordata,pstAppObject);
        }

    }
}


void heartbeatToJSON()
{
    cJSON *root_json = NULL;
    char *out = NULL;
    int len = 0;
    int ret = 0;
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
    if (pstDeviceObject)
    {
        root_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(root_json, "msgId", g_msgId_hearBeat++);
        cJSON_AddNumberToObject(root_json, "msgType", emMqttMsgTypeUp);
        cJSON_AddStringToObject(root_json, "deviceId", pstDeviceObject->m_deviceId);
        cJSON_AddNumberToObject(root_json, "deviceType", emDeviceCompositeSensor);
        cJSON_AddNumberToObject(root_json, "cmdType", emMqttCmdHeartBeat);
        out = cJSON_PrintUnformatted(root_json);
        len = strlen(out);
        if ( len >0)
        {
            ret = mq_publish(MQ_HEARTBEAT_TOPIC, out);
            rt_kprintf("MQTT MSG->topic:%s,ret:%d len:%d msg:%s\n",MQ_HEARTBEAT_TOPIC,ret,len, out);
        }
        cJSON_free(out);
        cJSON_Delete(root_json);
    }

    return 0;
}


void manage_comm_init(void)
{
    g_msgId_hearBeat = 0;
    g_msgId_control = 0;
    g_msgId_data = 0;

    g_mqttDmf.m_atemp =0;
    g_mqttDmf.m_btemp =0;
    g_mqttDmf.m_bio_value =0;
    g_mqttDmf.m_spo2=0;
    g_mqttDmf.m_bk=0;

    g_temp_mqttDmf.m_atemp =0;
    g_temp_mqttDmf.m_btemp =0;

    g_spo2_mqttDmf.m_spo2=0;
    g_spo2_mqttDmf.m_bk=0;

    g_bio_mqttDmf.m_bio_value =0;
}

void manage_comm_start(void)
{
    //mq_start();
    comm_mqtt_heartBeat();
    comm_mqtt_sensondata();
}

void manage_comm_stop(void)
{
    mq_stop();
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    pstAppObject->brun_mqtt = SD_FALSE;
    pstAppObject->brun_sensor = SD_FALSE;
}
