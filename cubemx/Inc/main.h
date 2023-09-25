/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define pump1_Pin GPIO_PIN_2
#define pump1_GPIO_Port GPIOB
#define pump2_Pin GPIO_PIN_7
#define pump2_GPIO_Port GPIOE
#define pump3_Pin GPIO_PIN_8
#define pump3_GPIO_Port GPIOE
#define pump4_Pin GPIO_PIN_9
#define pump4_GPIO_Port GPIOE
#define pump5_Pin GPIO_PIN_10
#define pump5_GPIO_Port GPIOE
#define valve1_Pin GPIO_PIN_11
#define valve1_GPIO_Port GPIOE
#define valve2_Pin GPIO_PIN_12
#define valve2_GPIO_Port GPIOE
#define valve3_Pin GPIO_PIN_13
#define valve3_GPIO_Port GPIOE
#define valve4_Pin GPIO_PIN_14
#define valve4_GPIO_Port GPIOE
#define valve5_Pin GPIO_PIN_15
#define valve5_GPIO_Port GPIOE
#define emerstop1_Pin GPIO_PIN_8
#define emerstop1_GPIO_Port GPIOD
#define emerstop2_Pin GPIO_PIN_9
#define emerstop2_GPIO_Port GPIOD
#define emerstop3_Pin GPIO_PIN_10
#define emerstop3_GPIO_Port GPIOD
#define emerstop4_Pin GPIO_PIN_11
#define emerstop4_GPIO_Port GPIOD
#define emerstop5_Pin GPIO_PIN_12
#define emerstop5_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
