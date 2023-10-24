/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-01-04     Administrator       the first version
 */
#include "inc/m_comm.h"
#include "inc/m_mqtt.h"
#include "cJSON.h"

static SdULong g_msgId_hearBeat;
static SdULong g_msgId_control;
static SdULong g_msgId_data;
SensorDataFrameDef mqttDmf;



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

SdInt comm_mqtt_subMsg(char* topic, char * message)
{
    rt_kprintf("MQTT MSG->topic:%s,msg:%s\n",topic,message);
    cJSON *root = NULL,*data = NULL;
    cJSON *msgType = NULL, *deviceId = NULL, *deviceType = NULL,*cmdType = NULL;
    cJSON *acupointId = NULL,*pressure = NULL;

    if ( rt_strcmp( topic,MQ_CONTROL_TOPIC) == 0)
    {
        root = cJSON_Parse(message);
        if(root)
        {
            msgType = cJSON_GetObjectItem(root, "msgType");
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
                            pstDeviceObject->m_check_acupointId = atoi(acupointId->valuestring);
                        }

                        if ( pressure){
                            pstDeviceObject->m_pressValue_param = atoi(pressure->valuestring);
                        }

                    }

                    if ( deviceType->valueint == emDevicePressControlSensor)
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
        cJSON_Delete(root);
    }

    return 0;
}


SdInt comm_mqtt_msg(const UTMsgDef *pMsg, const void *pContent) {
    //cJSON *root_json = cJSON_CreateObject();
    //char *out = NULL;
    //int len = 0;
    //int ret = 0;
    LPSensorDataFrameDef pdmf = (LPSensorDataFrameDef)pContent;
    //SensorDataFrameDef mqttDmf;
    if (pMsg->usMsgID == emMqttMsgBaseData) {
        if (pContent != SD_NULL) {
            //LPSensorDataFrameDef pdmf = (LPSensorDataFrameDef)pContent;
            if (pdmf != NULL) {
                //SensorDataFrameDef mqttDmf;
                mqttDmf.m_atemp = pdmf->m_atemp;
                mqttDmf.m_btemp = pdmf->m_btemp;
            
            }
        }
    } else if (pMsg->usMsgID == emMqttMsgSpo2Data) {
        if (pContent != SD_NULL) {
            
            if (pdmf != NULL) {
                
                mqttDmf.m_spo2 = pdmf->m_spo2;
                mqttDmf.m_bk = pdmf->m_bk;

            }
        }
    } else if (pMsg->usMsgID == emMqttMsgBioData) {
        if (pContent != SD_NULL) {
            LPSensorDataFrameDef pbmf = (LPSensorDataFrameDef)pContent;
            if (pbmf != NULL) {
                
                mqttDmf.m_bio_ampere = pbmf->m_bio_ampere;
                mqttDmf.m_bio_voltage = pbmf->m_bio_voltage;
                mqttDmf.m_bio_value = pbmf->m_bio_value;

            }
        }
    }


    return 0;
}  
SdInt comm_mqtt_msg_publish()
{
    cJSON *root_json = cJSON_CreateObject();
        char *out = NULL;
        int len = 0;
        int ret = 0;
        sensorDataToJSON(&mqttDmf, root_json);  //2023.9.27
        out = cJSON_PrintUnformatted(root_json);
        len = strlen(out);
        //rt_kprintf("***************************len:%d\n",len);  //2023.9.27，打印长度

        if (len > 0) {
            ret = mq_publish(MQ_DATA_TOPIC, out);

            //cnt_sensor ++;
            //rt_kprintf("****cnt_sensor:%d\n",cnt_sensor);  //2023.9.27，打印接收传感器参数计数
        }
        cJSON_free(out);
        cJSON_Delete(root_json);
        return ret;

}



void sensorDataToJSON(SensorDataFrameDef *dmf, cJSON *root_json)
{
    char tValue_temp[8], tValue_spo2[8], tValue_bio[8];
    cJSON *data_json = NULL;
    cJSON *phase_array = NULL;
    LPDeviceObjectDef pstDeviceObject = device_ctrl_object_get();
    if (pstDeviceObject)
    {
        cJSON_AddNumberToObject(root_json, "msgId", g_msgId_data++);
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
        sprintf(tValue_temp, "%.1f", temp_Value);
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
}

void manage_comm_start(void)
{
    //mq_start();
    comm_mqtt_heartBeat();
}

void manage_comm_stop(void)
{
    mq_stop();
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    pstAppObject->brun_mqtt = SD_FALSE;
}
