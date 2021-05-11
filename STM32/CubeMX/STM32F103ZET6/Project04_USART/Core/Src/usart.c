/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

// 头文件
#include <string.h>

// 宏定义
#define HUART1_BUF_MAX_LEN 200

// 变量
uint8_t huart1_dat;
uint8_t huart1_recbuf[HUART1_BUF_MAX_LEN];
uint16_t huart1_sta = 0;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint16_t i;
    uint16_t len;
    if (huart -> Instance == USART1) {
        if ((huart1_sta & 0x8000) == 0) {
            if ((huart1_sta & 0x4000) == 0) {
                if (huart1_dat == 0x0D) {
                    huart1_sta |= 0x4000;
                } else {
                    huart1_recbuf[huart1_sta] = huart1_dat;
                    huart1_sta++;
                    if (huart1_sta > HUART1_BUF_MAX_LEN - 1) {
                        huart1_sta = 0;
                        memset(huart1_recbuf, 0, sizeof(huart1_recbuf));
                    }
                }                
            } else {
                if (huart1_dat == 0x0A) {
                    huart1_sta |= 0x8000;
                } else {
                    huart1_sta = 0;
                }
            }
        } else {
            len = huart1_sta & 0x3FFF;
            HAL_UART_Transmit(&huart1, "您输入的是: ", sizeof("您输入的是: "), 1000);
            for (i = 0; i < len; i++) {
                HAL_UART_Transmit(&huart1, &huart1_recbuf[i], 1, 1000);
            }
            HAL_UART_Transmit(&huart1, "\r\n", sizeof("\r\n"), 1000);
            huart1_sta = 0;
            memset(huart1_recbuf, 0, sizeof(huart1_recbuf));
        }        
    }
    HAL_UART_Receive_IT(&huart1, &huart1_dat, 1);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
