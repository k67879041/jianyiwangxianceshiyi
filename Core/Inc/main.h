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

#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"

#include "stm32f4xx_ll_exti.h"

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
#define OUT_A1_Pin GPIO_PIN_2
#define OUT_A1_GPIO_Port GPIOE
#define OUT_A2_Pin GPIO_PIN_3
#define OUT_A2_GPIO_Port GPIOE
#define OUT_A3_Pin GPIO_PIN_4
#define OUT_A3_GPIO_Port GPIOE
#define OUT_A4_Pin GPIO_PIN_5
#define OUT_A4_GPIO_Port GPIOE
#define OUT_A5_Pin GPIO_PIN_6
#define OUT_A5_GPIO_Port GPIOE
#define OUT_B9_Pin GPIO_PIN_13
#define OUT_B9_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_6
#define KEY1_GPIO_Port GPIOF
#define KEY2_Pin GPIO_PIN_7
#define KEY2_GPIO_Port GPIOF
#define OUT_B1_Pin GPIO_PIN_8
#define OUT_B1_GPIO_Port GPIOF
#define Relay_switch_1_Pin GPIO_PIN_9
#define Relay_switch_1_GPIO_Port GPIOF
#define LCD_BL_Pin GPIO_PIN_10
#define LCD_BL_GPIO_Port GPIOF
#define AD7606_CS_Pin GPIO_PIN_0
#define AD7606_CS_GPIO_Port GPIOC
#define AD7606_RST_Pin GPIO_PIN_1
#define AD7606_RST_GPIO_Port GPIOC
#define OUT_A9_Pin GPIO_PIN_0
#define OUT_A9_GPIO_Port GPIOA
#define T_CLK_Pin GPIO_PIN_5
#define T_CLK_GPIO_Port GPIOA
#define AD7606_BUSY_Pin GPIO_PIN_4
#define AD7606_BUSY_GPIO_Port GPIOC
#define AD7606_BUSY_EXTI_IRQn EXTI4_IRQn
#define AD7606_CONVA_Pin GPIO_PIN_5
#define AD7606_CONVA_GPIO_Port GPIOC
#define T_CS_Pin GPIO_PIN_0
#define T_CS_GPIO_Port GPIOB
#define T_MOSI_Pin GPIO_PIN_1
#define T_MOSI_GPIO_Port GPIOB
#define T_MISO_Pin GPIO_PIN_2
#define T_MISO_GPIO_Port GPIOB
#define T_PEN_Pin GPIO_PIN_11
#define T_PEN_GPIO_Port GPIOF
#define AD7606_SCK_Pin GPIO_PIN_10
#define AD7606_SCK_GPIO_Port GPIOB
#define AD7606_MISO_Pin GPIO_PIN_14
#define AD7606_MISO_GPIO_Port GPIOB
#define OUT_A8_Pin GPIO_PIN_15
#define OUT_A8_GPIO_Port GPIOB
#define AD9959_Reset_Pin GPIO_PIN_6
#define AD9959_Reset_GPIO_Port GPIOG
#define AD9959_PWR_Pin GPIO_PIN_7
#define AD9959_PWR_GPIO_Port GPIOG
#define SPI1_NSS_Pin GPIO_PIN_8
#define SPI1_NSS_GPIO_Port GPIOG
#define AD9959_UPDATE_Pin GPIO_PIN_8
#define AD9959_UPDATE_GPIO_Port GPIOA
#define AD9959_CS_Pin GPIO_PIN_11
#define AD9959_CS_GPIO_Port GPIOA
#define AD9959_SCLK_Pin GPIO_PIN_12
#define AD9959_SCLK_GPIO_Port GPIOA
#define OUT_A7_Pin GPIO_PIN_11
#define OUT_A7_GPIO_Port GPIOC
#define OUT_A6_Pin GPIO_PIN_12
#define OUT_A6_GPIO_Port GPIOC
#define AD9959_SDIO3_Pin GPIO_PIN_3
#define AD9959_SDIO3_GPIO_Port GPIOD
#define AD9959_PS1_Pin GPIO_PIN_6
#define AD9959_PS1_GPIO_Port GPIOD
#define AD9959_PS0_Pin GPIO_PIN_7
#define AD9959_PS0_GPIO_Port GPIOD
#define AD9959_PS2_Pin GPIO_PIN_9
#define AD9959_PS2_GPIO_Port GPIOG
#define AD9959_PS3_Pin GPIO_PIN_11
#define AD9959_PS3_GPIO_Port GPIOG
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOG
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOG
#define AD9959_SDIO0_Pin GPIO_PIN_15
#define AD9959_SDIO0_GPIO_Port GPIOG
#define AD9959_SDIO2_Pin GPIO_PIN_6
#define AD9959_SDIO2_GPIO_Port GPIOB
#define AD9959_SDIO1_Pin GPIO_PIN_7
#define AD9959_SDIO1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
