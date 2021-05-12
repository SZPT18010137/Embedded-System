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
// ͷ�ļ�
#include <stdio.h>
#include <string.h>

// �궨��
#define UART2_BUF_LEN 200 // ����2���ݵĳ���

// ����
uint8_t huart2_data;               // ����2����
uint8_t huart2_buf[UART2_BUF_LEN]; // ����2���ݻ�����
uint16_t huart2_sta = 0x0000;      // ����2���ݱ��

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

// ����2�����жϷ�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // ����
    uint8_t huart2_sendbuf[UART2_BUF_LEN]; // ���ͻ�����
    // ����Ǵ���2
    if(huart -> Instance == USART2) { 
        // �������û�н������
        if( (huart2_sta  & 0x8000) == 0) { 
            // ���û�н��յ�0x0D
            if( (huart2_sta & 0x4000) == 0) { 
                // ������յ���0x0D
                if(huart2_data == 0x0D) {
                    huart2_sta |= 0x4000; // ��ǽ��յ���0x0D
                // ������ܵ��Ĳ���0x0D
                } else {
                    huart2_buf[huart2_sta] = huart2_data; // �����ݴ洢�ڻ�������
                    huart2_sta++; // ��������
                    // ������ȳ��������ֵ
                    if (huart2_sta > UART2_BUF_LEN - 1) {
                        huart2_sta = 0x0000; // ʧ��
                    }
                }
            // ������յ���0x0D
            } else { 
                // ������յ�0x0A
                if(huart2_data == 0x0A) { 
                    huart2_sta  |= 0x8000; // ��ǳɹ�
                // ���û�н��յ�0x0A
                } else {
                   huart2_sta = 0x0000; // ʧ��
                }
            }
        // �������2�������
				} else {
            memset(huart2_sendbuf, 0, sizeof(huart2_sendbuf)); // ������ͻ�����
            sprintf(huart2_sendbuf, "[Message] You had entered: %s\r\n", huart2_buf); // �ѽ��ջ����������ݴ浽���ͻ�����
            HAL_UART_Transmit(&huart2, huart2_sendbuf, sizeof(huart2_sendbuf), 1000); // ��������
            huart2_sta = 0x0000; // ��ǳɹ�
						memset(huart2_buf, 0, sizeof(huart2_buf)); // ������ͻ�����
				}
    }
    HAL_UART_Receive_IT(&huart2, &huart2_data, 1); // ����һ������2������
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
