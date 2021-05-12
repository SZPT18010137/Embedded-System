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

// 头文件
#include <stdio.h>
#include <string.h>

// 宏定义
#define UART2_BUF_LEN 200 // 串口2数据的长度

// 变量
uint8_t huart2_data;               // 串口2数据
uint8_t huart2_buf[UART2_BUF_LEN]; // 串口2数据缓冲区
uint16_t huart2_sta = 0x0000;      // 串口2数据标记

// 函数
/*
 * 函数名：  串口2接收中断服务函数
 * 输入参数：UART_HandleTypeDef *huart 串口指针
 * 返回值：  无
 * 描述：    串口2的中断服务函数，具有收发功能
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t huart2_sendbuf[UART2_BUF_LEN];
    char *rec;
    char pwm_number[3];
    uint8_t pwm_duty;
    
    if(huart -> Instance == USART2) { 
        /* 串口未接收成功 */
        if( (huart2_sta  & 0x8000) == 0) { 
            if( (huart2_sta & 0x4000) == 0) {
                if(huart2_data == 0x0D) {
                    huart2_sta |= 0x4000; 
                } else {
                    huart2_buf[huart2_sta] = huart2_data; 
                    huart2_sta++;
                    if (huart2_sta > UART2_BUF_LEN - 1) {
                        huart2_sta = 0x0000;
                    }
                }
            } else { 
                if(huart2_data == 0x0A) { 
                    huart2_sta  |= 0x8000; 
                } else {
                   huart2_sta = 0x0000;
                }
            }
        /* 串口接收完成 */
        } else {
            /* 成功接收到信息 */
            memset(huart2_sendbuf, 0, sizeof(huart2_sendbuf));
            sprintf(huart2_sendbuf, (uint8_t *)"[Message] USART2 recieve message: %s.\r\n", huart2_buf); // 提示：你输入的是
            HAL_UART_Transmit(&huart2, huart2_sendbuf, sizeof(huart2_sendbuf), 1000);
            /* 通道1改变PWM命令 */
            if (rec = strstr(huart2_buf, "T1C1PWM_")) {
                strncpy(pwm_number, rec + 8, 3);
                pwm_duty = ((pwm_number[0] - 48 ) * 100) + ((pwm_number[1] - 48 ) * 10) + (pwm_number[2] - 48 );
                if (pwm_duty < 101) {
                    TIM1 -> CCR1 = pwm_duty;
                    sprintf(huart2_sendbuf, (uint8_t *)"[Command] Change TIM1 Channel1 PWM: %d.\r\n", pwm_duty); // 命令：改变TIM1通道1PWM
                    HAL_UART_Transmit(&huart2, huart2_sendbuf, sizeof(huart2_sendbuf), 1000); // 发送数据
                } else {
                    sprintf(huart2_sendbuf, (uint8_t *)"[Error] Incorrect PWM value.\r\n"); // 报错：不正确的PWM范围
                    HAL_UART_Transmit(&huart2, huart2_sendbuf, sizeof(huart2_sendbuf), 1000); // 发送数据
                }                    
            }
            huart2_sta = 0x0000; 
            memset(huart2_buf, 0, sizeof(huart2_buf));
        }
    }
    HAL_UART_Receive_IT(&huart2, &huart2_data, 1);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
