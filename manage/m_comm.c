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

void baseDataToJSON(DataFrameDef *dmf, cJSON *root_json);
void bioDataToJSON(BioFrameDef *bmf, cJSON *root_json);
void spo2DataToJSON(Spo2FrameDef *dmf, cJSON *root_json);
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
            if ( pstDeviceObject && pstDeviceObject->m_mqttStatus == 1)
            {
                heartbeatToJSON();
                rt_kprintf("[MQTT Module]-> heartbeat thread run\n");
            }
            rt_thread_mdelay(1000*10);
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

SdInt comm_mqtt_msg(const UTMsgDef *pMsg, const void *pContent)
{
    cJSON *root_json = NULL;
    char *out = NULL;
	int len = 0;
	int ret = 0;

	root_json = cJSON_CreateObject();

    if( pMsg->usMsgID == emMqttMsgBaseData)
    {
        if ( pContent != SD_NULL)
        {
            LPDataFrameDef pdmf = (LPDataFrameDef)pContent;
            if ( pdmf != NULL)
            {
                //rt_kprintf("[Mqtt Module] handle data %d ,%d\n",pdmf->m_atemp,pdmf->m_btemp);

                DataFrameDef mqttDmf;
                mqttDmf.m_atemp = pdmf->m_atemp;
                mqttDmf.m_btemp = pdmf->m_btemp;

                baseDataToJSON(&mqttDmf,root_json);
                out = cJSON_PrintUnformatted(root_json);
                len = strlen(out);
                if ( len >0)
                {
                    ret = mq_publish(MQ_DATA_TOPIC, out);
                    rt_kprintf("MQTT MSG->topic:%s,ret:%d len:%d msg:%s\n",MQ_DATA_TOPIC,ret,len, out);
                }
                cJSON_free(out);
            }

        }
    }
    else if ( pMsg->usMsgID == emMqttMsgSpo2Data)
    {
        if ( pContent != SD_NULL)
        {
            LPSpo2FrameDef pdmf = (LPSpo2FrameDef)pContent;
            if ( pdmf != NULL)
            {
                //rt_kprintf("[Mqtt Module] handle data %d\n",pdmf->m_spo2);

                Spo2FrameDef mqttDmf;
                mqttDmf.m_spo2 = pdmf->m_spo2;

                spo2DataToJSON(&mqttDmf,root_json);
                out = cJSON_PrintUnformatted(root_json);
                len = strlen(out);
                if ( len >0)
                {
                    ret = mq_publish(MQ_DATA_TOPIC, out);
                    rt_kprintf("MQTT MSG->topic:%s,ret:%d len:%d msg:%s\n",MQ_DATA_TOPIC,ret,len, out);
                }
                cJSON_free(out);
            }

        }
    }
    else if ( pMsg->usMsgID == emMqttMsgBioData)
    {
        if ( pContent != SD_NULL)
        {
            LPBioFrameDef pbmf = (LPBioFrameDef)pContent;
            if ( pbmf != NULL)
            {
                //rt_kprintf("[Mqtt Module] handle data %d ,%d ,%d \n",pbmf->m_bio_ampere,pbmf->m_bio_value,pbmf->m_bio_voltage);

                BioFrameDef mqttDmf;
                mqttDmf.m_bio_ampere = pbmf->m_bio_ampere;
                mqttDmf.m_bio_voltage = pbmf->m_bio_voltage;
                mqttDmf.m_bio_value = pbmf->m_bio_value;

                bioDataToJSON(&mqttDmf,root_json);
                out = cJSON_PrintUnformatted(root_json);
                len = strlen(out);
                if ( len >0)
                {
                    ret = mq_publish(MQ_DATA_TOPIC, out);
                    rt_kprintf("MQTT MSG->topic:%s,ret:%d len:%d msg:%s\n",MQ_DATA_TOPIC,ret,len, out);
                }
                cJSON_free(out);
            }
        }
    }
    cJSON_Delete(root_json);
    return 0;
}

void baseDataToJSON(DataFrameDef *dmf, cJSON *root_json)
{
    char tValue[8];
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
        data_json = cJSON_CreateObject();
        //cJSON_AddStringToObject(data_json, "timeStamp", "2023-05-05 10:55:05");
        memset(tValue,0,8);
        sprintf(tValue,"%d",dmf->m_btemp);
        if ( strlen(tValue) >0){
            sprintf(tValue,"%d.%c",dmf->m_atemp,tValue[0]);
        }else{
            sprintf(tValue,"%d.%d",dmf->m_atemp,0);
        }
        
        cJSON_AddStringToObject(data_json, "tempValue", tValue);
        cJSON_AddItemToArray(phase_array, data_json);

        cJSON_AddItemToObject(root_json, "data", phase_array);
    }

}

void spo2DataToJSON(Spo2FrameDef *dmf, cJSON *root_json)
{
    char tValue[8];
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
        data_json = cJSON_CreateObject();
        //cJSON_AddStringToObject(data_json, "timeStamp", "2023-05-05 10:55:05");
        memset(tValue,0,8);
        sprintf(tValue,"%d",dmf->m_spo2);
        cJSON_AddStringToObject(data_json, "spo2Value", tValue);
        cJSON_AddItemToArray(phase_array, data_json);

        cJSON_AddItemToObject(root_json, "data", phase_array);
    }

}

void bioDataToJSON(BioFrameDef *bmf, cJSON *root_json)
{
    char tValue[8];
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
       data_json = cJSON_CreateObject();
       //cJSON_AddStringToObject(data_json, "timeStamp", "2023-05-05 10:55:05");
       memset(tValue,0,8);
       sprintf(tValue,"%d",bmf->m_bio_value);
       cJSON_AddStringToObject(data_json, "bioValue",tValue);
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
    mq_start();
    comm_mqtt_heartBeat();
}

void manage_comm_stop(void)
{
    mq_stop();
    LPAppObjectDef pstAppObject = app_ctrl_object_get();
    pstAppObject->brun_mqtt = SD_FALSE;
}
