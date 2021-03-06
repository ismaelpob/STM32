/*
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define TRUE 1
#define FALSE 0

/* Exported macro ------------------------------------------------------------*/
#define USE_FULL_ASSERT

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define USART_TX_Pin 				GPIO_PIN_2
#define USART_TX_GPIO_Port 			GPIOA
#define USART_RX_Pin 				GPIO_PIN_3
#define USART_RX_GPIO_Port 			GPIOA


#endif /* __MAIN_H */
