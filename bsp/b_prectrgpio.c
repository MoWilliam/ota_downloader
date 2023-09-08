
/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-0605    WangWei       the first version
 */


/*
做重新变更：1.设置GPIO加压和减压操作
*/

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "ut/inc/ut_thread.h"
#include "inc/b_prectrgpio.h"


ALIGN(RT_ALIGN_SIZE)


void bsp_PreCtr_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(pump5_GPIO_Port, pump5_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, pump1_Pin|pump2_Pin|pump3_Pin|pump4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, valve1_Pin|valve2_Pin|valve3_Pin|valve4_Pin|valve5_Pin, GPIO_PIN_RESET);
    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = pump5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(pump5_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                            PEPin PEPin PEPin PEPin
                            PEPin */
    GPIO_InitStruct.Pin = pump1_Pin|pump2_Pin|pump3_Pin|pump4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    //rt_kprintf(" /b_prectrgpio.c/ pump and valve GPIO init\n");  //输出打印

	GPIO_InitStruct.Pin = valve1_Pin|valve2_Pin|valve3_Pin|valve4_Pin|valve5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}




void PUMP_on(int device_id)  //气泵的打开，变高电平
{
	switch(device_id)
	{
		case PressureSensor1:
		HAL_GPIO_WritePin(pump1_GPIO_Port, pump1_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor2:
		HAL_GPIO_WritePin(pump2_GPIO_Port, pump2_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor3:
		HAL_GPIO_WritePin(pump3_GPIO_Port, pump3_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor4:
		HAL_GPIO_WritePin(pump4_GPIO_Port, pump4_Pin, GPIO_PIN_SET);
        case PressureSensor5:
		HAL_GPIO_WritePin(pump5_GPIO_Port, pump5_Pin, GPIO_PIN_SET);
		break;
        
		default:
		//do nothing
		break;
	}
}

void PUMP_off(int device_id)  //气泵的关闭，停止工作，变低电平
{
	switch(device_id)
	{
		case PressureSensor1:
		HAL_GPIO_WritePin(pump1_GPIO_Port, pump1_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor2:
		HAL_GPIO_WritePin(pump2_GPIO_Port, pump2_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor3:
		HAL_GPIO_WritePin(pump3_GPIO_Port, pump3_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor4:
		HAL_GPIO_WritePin(pump4_GPIO_Port, pump4_Pin, GPIO_PIN_RESET);
        case PressureSensor5:
		HAL_GPIO_WritePin(pump5_GPIO_Port, pump5_Pin, GPIO_PIN_RESET);
		break;
        
		default:
		//do nothing
		break;
	}

}

void VALVE_on(int device_id)
{
	switch(device_id)
	{
		case PressureSensor1:
		HAL_GPIO_WritePin(valve1_GPIO_Port, valve1_Pin, GPIO_PIN_SET);
		break;
		break;
		case PressureSensor2:
		HAL_GPIO_WritePin(valve2_GPIO_Port, valve2_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor3:
		HAL_GPIO_WritePin(valve3_GPIO_Port, valve3_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor4:
		HAL_GPIO_WritePin(valve1_GPIO_Port, valve1_Pin, GPIO_PIN_SET);
        case PressureSensor5:
		HAL_GPIO_WritePin(valve1_GPIO_Port, valve1_Pin, GPIO_PIN_SET);
		break;
        
		default:
		//do nothing
		break;
	}

}



void VALVE_off(int device_id)
{
	switch(device_id)
	{
		case PressureSensor1:
		HAL_GPIO_WritePin(valve1_GPIO_Port, valve1_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor2:
		HAL_GPIO_WritePin(valve2_GPIO_Port, valve2_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor3:
		HAL_GPIO_WritePin(valve3_GPIO_Port, valve3_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor4:
		HAL_GPIO_WritePin(valve4_GPIO_Port, valve4_Pin, GPIO_PIN_RESET);
        case PressureSensor5:
		HAL_GPIO_WritePin(valve5_GPIO_Port, valve5_Pin, GPIO_PIN_RESET);
		break;
        
		default:
		//do nothing
		break;
	}
}





void bsp_PreCtr_GPIO_start(int device_id)
{
    PUMP_on(device_id);
    VALVE_off(device_id);

}

void bsp_PreCtr_GPIO_stop(int device_id)
{
    PUMP_off(device_id);
    VALVE_on(device_id);

}







