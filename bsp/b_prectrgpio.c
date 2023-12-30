
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
gpio口的初始化， 加压减压功能实现
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
    HAL_GPIO_WritePin(pump_GPIO_Port, pump1_Pin|pump2_Pin|pump3_Pin|pump4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(valve_GPIO_Port, valve1_Pin|valve2_Pin|valve3_Pin|valve4_Pin|valve5_Pin, GPIO_PIN_RESET);
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
    HAL_GPIO_Init(pump_GPIO_Port, &GPIO_InitStruct);
    //rt_kprintf(" /b_prectrgpio.c/ pump and valve GPIO init\n");  //输出打印

	GPIO_InitStruct.Pin = valve1_Pin|valve2_Pin|valve3_Pin|valve4_Pin|valve5_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(valve_GPIO_Port, &GPIO_InitStruct);


    //LED
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

void LED_on(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

}

void PUMP_on(int device_id)  //气泵的打开，变高电平
{
	switch(device_id)
	{
		case PressureSensor1:
		HAL_GPIO_WritePin(pump_GPIO_Port, pump1_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor2:
		HAL_GPIO_WritePin(pump_GPIO_Port, pump2_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor3:
		HAL_GPIO_WritePin(pump_GPIO_Port, pump3_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor4:
		HAL_GPIO_WritePin(pump_GPIO_Port, pump4_Pin, GPIO_PIN_SET);
		break;
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
		HAL_GPIO_WritePin(pump_GPIO_Port, pump1_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor2:
		HAL_GPIO_WritePin(pump_GPIO_Port, pump2_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor3:
		HAL_GPIO_WritePin(pump_GPIO_Port, pump3_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor4:
		HAL_GPIO_WritePin(pump_GPIO_Port, pump4_Pin, GPIO_PIN_RESET);
		break;
        case PressureSensor5:
		HAL_GPIO_WritePin(pump5_GPIO_Port, pump5_Pin, GPIO_PIN_RESET);
		break;
        case AllPressureSensor: // 关闭所有气泵
        HAL_GPIO_WritePin(pump_GPIO_Port, pump1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pump_GPIO_Port, pump2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pump_GPIO_Port, pump3_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pump_GPIO_Port, pump4_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(pump_GPIO_Port, pump5_Pin, GPIO_PIN_RESET);
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
		HAL_GPIO_WritePin(valve_GPIO_Port, valve1_Pin, GPIO_PIN_SET);
		break;
		break;
		case PressureSensor2:
		HAL_GPIO_WritePin(valve_GPIO_Port, valve2_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor3:
		HAL_GPIO_WritePin(valve_GPIO_Port, valve3_Pin, GPIO_PIN_SET);
		break;
		case PressureSensor4:
		HAL_GPIO_WritePin(valve_GPIO_Port, valve4_Pin, GPIO_PIN_SET);
		break;
        case PressureSensor5:
		HAL_GPIO_WritePin(valve_GPIO_Port, valve5_Pin, GPIO_PIN_SET);
		break;
        case AllPressureSensor: // 打开所有阀门
        HAL_GPIO_WritePin(valve_GPIO_Port, valve1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(valve_GPIO_Port, valve2_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(valve_GPIO_Port, valve3_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(valve_GPIO_Port, valve4_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(valve_GPIO_Port, valve5_Pin, GPIO_PIN_SET);
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
		HAL_GPIO_WritePin(valve_GPIO_Port, valve1_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor2:
		HAL_GPIO_WritePin(valve_GPIO_Port, valve2_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor3:
		HAL_GPIO_WritePin(valve_GPIO_Port, valve3_Pin, GPIO_PIN_RESET);
		break;
		case PressureSensor4:
		HAL_GPIO_WritePin(valve_GPIO_Port, valve4_Pin, GPIO_PIN_RESET);
		break;
        case PressureSensor5:
		HAL_GPIO_WritePin(valve_GPIO_Port, valve5_Pin, GPIO_PIN_RESET);
		break;
        

		default:
		//do nothing
		break;
	}
}

//加压功能
void bsp_PreCtr_GPIO_start(int device_id)  
{
    PUMP_on(device_id);
    VALVE_off(device_id);

}

//停止加压功能
void bsp_PreCtr_GPIO_hold(int device_id)  
{
    PUMP_off(device_id);
    VALVE_off(device_id);

}

//泄压功能
void bsp_PreCtr_GPIO_stop(int device_id)
{
    PUMP_off(device_id);
    VALVE_on(device_id);

}

void bsp_PreCtr_GPIO_stopall(void)
{
    PUMP_off(AllPressureSensor);
    VALVE_on(AllPressureSensor);

}

/*void bsp_PreCtr_GPIO_EmerStop(void)
{
    PUMP_off(AllPressureSensor);
    VALVE_on(AllPressureSensor);

}*/









