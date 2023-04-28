/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-02-13     Administrator       the first version
 */
#ifndef MANAGE_INC_M_MQTT_H_
#define MANAGE_INC_M_MQTT_H_

int mq_start(void);
int mq_stop(void);
int mq_publish(char *topic ,char *message);
int mq_subscribe(char *topic);
int mq_unsubscribe(char *topic);

#endif /* MANAGE_INC_M_MQTT_H_ */
