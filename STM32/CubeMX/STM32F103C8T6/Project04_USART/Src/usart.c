/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#include <stdio.h>
#include <string.h>

// 宏定义
#define UART2_BUF_LEN 200 // 串口2数据的长度

// 变量
uint8_t huart2_data;               // 串口2数据
uint8_t huart2_buf[UART2_BUF_LEN]; // 串口2数据缓冲区
uint16_t huart2_sta = 0x0000;      // 串口2数据标记

/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

// 串口2接收中断服务函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 变量
    uint8_t huart2_sendbuf[UART2_BUF_LEN]; // 发送缓冲区
    // 如果是串口2
    if(huart -> Instance == USART2) { 
        // 如果串口没有接收完成
        if( (huart2_sta  & 0x8000) == 0) { 
            // 如果没有接收到0x0D
            if( (huart2_sta & 0x4000) == 0) { 
                // 如果接收到了0x0D
                if(huart2_data == 0x0D) {
                    huart2_sta |= 0x4000; // 标记接收到了0x0D
                // 如果接受到的不是0x0D
                } else {
                    huart2_buf[huart2_sta] = huart2_data; // 将数据存储在缓冲区中
                    huart2_sta++; // 长度增加
                    // 如果长度超过了最大值
                    if (huart2_sta > UART2_BUF_LEN - 1) {
                        huart2_sta = 0x0000; // 失败
                    }
                }
            // 如果接收到了0x0D
            } else { 
                // 如果接收到0x0A
                if(huart2_data == 0x0A) { 
                    huart2_sta  |= 0x8000; // 标记成功
                // 如果没有接收到0x0A
                } else {
                   huart2_sta = 0x0000; // 失败
                }
            }
        // 如果串口2接收完成
				} else {
            memset(huart2_sendbuf, 0, sizeof(huart2_sendbuf)); // 清除发送缓冲区
            sprintf(huart2_sendbuf, "[Message] You had entered: %s\r\n", huart2_buf); // 把接收缓冲区的数据存到发送缓冲区
            HAL_UART_Transmit(&huart2, huart2_sendbuf, sizeof(huart2_sendbuf), 1000); // 发送数据
            huart2_sta = 0x0000; // 标记成功
						memset(huart2_buf, 0, sizeof(huart2_buf)); // 清除发送缓冲区
				}
    }
    HAL_UART_Receive_IT(&huart2, &huart2_data, 1); // 接收一个串口2的数据
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
