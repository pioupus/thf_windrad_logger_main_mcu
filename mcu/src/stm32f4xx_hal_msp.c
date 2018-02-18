/**
 ******************************************************************************
 * File Name          : stm32l1xx_hal_msp.c
 * Description        : This file provides code for the MSP Initialization
 *                      and de-Initialization codes.
 ******************************************************************************
 *
 * COPYRIGHT(c) 2016 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without
 *modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *notice,
 *      this list of conditions and the following disclaimer in the
 *documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "assert.h"
#include "main.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) {
    /* USER CODE BEGIN MspInit 0 */

    /* USER CODE END MspInit 0 */

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/
    /* MemoryManagement_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    /* BusFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    /* UsageFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    /* DebugMonitor_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    /* SysTick_IRQn interrupt configuration */

    // is handled in port.c from freertos
    /* USER CODE BEGIN MspInit 1 */

    /* USER CODE END MspInit 1 */
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc) {
    GPIO_InitTypeDef GPIO_InitStruct;
    if (hadc->Instance == ADC1) {
        /* USER CODE BEGIN ADC1_MspInit 0 */

        /* USER CODE END ADC1_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_ADC1_CLK_ENABLE();

        HAL_NVIC_SetPriority(ADC_IRQn, ISR_PRIORITY_ADC, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);

        // gpio init happened in borad.c

        /* USER CODE BEGIN ADC1_MspInit 1 */

        /* USER CODE END ADC1_MspInit 1 */
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
        __HAL_RCC_ADC1_CLK_DISABLE();
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    GPIO_InitTypeDef GPIO_InitStruct;

    if (hi2c->Instance == I2C2) {
        /* USER CODE BEGIN I2C2_MspInit 0 */

        /* USER CODE END I2C2_MspInit 0 */

        /**I2C2 GPIO Configuration
         PB10     ------> I2C2_SCL
         PB11     ------> I2C2_SDA
         */
        // gpio init happened in board.c
        /* Peripheral clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();
        /* USER CODE BEGIN I2C2_MspInit 1 */

        /* USER CODE END I2C2_MspInit 1 */
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C2) {
        /* USER CODE BEGIN I2C2_MspDeInit 0 */

        /* USER CODE END I2C2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_I2C2_CLK_DISABLE();

        /**I2C2 GPIO Configuration
         PB10     ------> I2C2_SCL
         PB11     ------> I2C2_SDA
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);
    }
    /* USER CODE BEGIN I2C2_MspDeInit 1 */

    /* USER CODE END I2C2_MspDeInit 1 */
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc) {
    if (hrtc->Instance == RTC) {
        __HAL_RCC_RTC_ENABLE();
    }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc) {
    if (hrtc->Instance == RTC) {
        /* USER CODE BEGIN RTC_MspDeInit 0 */

        /* USER CODE END RTC_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();
    }
    /* USER CODE BEGIN RTC_MspDeInit 1 */

    /* USER CODE END RTC_MspDeInit 1 */
}

void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == I2S3) {
        /* USER CODE BEGIN SPI2_MspInit 0 */

        /* USER CODE END SPI2_MspInit 0 */
        /* Peripheral clock enable */
        I2S3_CLK_ENABLE();
    }
}

void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == I2S3) {
        /* USER CODE BEGIN SPI2_MspDeInit 0 */

        /* USER CODE END SPI2_MspDeInit 0 */
        /* Peripheral clock disable */
        I2S3_CLK_DISABLE();

        /**SPI2 GPIO Configuration
         PB12     ------> SPI2_NSS
         PB13     ------> SPI2_SCK
         PB15     ------> SPI2_MOSI
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15);
    }
    /* USER CODE BEGIN SPI2_MspDeInit 1 */

    /* USER CODE END SPI2_MspDeInit 1 */
}

#if 0
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {

        __HAL_RCC_TIM2_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM2_IRQn, ISR_PRIORITY_HL_TIME_BASE, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}
#endif

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim_ic) {
    GPIO_InitTypeDef GPIO_InitStruct;
    if (htim_ic->Instance == TIM4) {
        /* USER CODE BEGIN TIM4_MspInit 0 */

        /* USER CODE END TIM4_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM4_CLK_ENABLE();

        /**TIM4 GPIO Configuration
         PB6     ------> TIM4_CH1
         */
        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM4_MspInit 1 */

        /* USER CODE END TIM4_MspInit 1 */
    } else if (htim_ic->Instance == TIM9) {
        /* USER CODE BEGIN TIM9_MspInit 0 */

        /* USER CODE END TIM9_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM9_CLK_ENABLE();

        /**TIM9 GPIO Configuration
         PA2     ------> TIM9_CH1
         */
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM9_MspInit 1 */

        /* USER CODE END TIM9_MspInit 1 */
    } else if (htim_ic->Instance == TIM10) {
        /* USER CODE BEGIN TIM10_MspInit 0 */

        /* USER CODE END TIM10_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM10_CLK_ENABLE();

        /**TIM10 GPIO Configuration
         PA6     ------> TIM10_CH1
         */
        GPIO_InitStruct.Pin = GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM10;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM10_MspInit 1 */

        /* USER CODE END TIM10_MspInit 1 */
    } else if (htim_ic->Instance == TIM11) {
        /* USER CODE BEGIN TIM11_MspInit 0 */

        /* USER CODE END TIM11_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_TIM11_CLK_ENABLE();

        /**TIM11 GPIO Configuration
         PA7     ------> TIM11_CH1
         */
        GPIO_InitStruct.Pin = GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF3_TIM11;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USER CODE BEGIN TIM11_MspInit 1 */

        /* USER CODE END TIM11_MspInit 1 */
    }
}

void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim_ic) {
    if (htim_ic->Instance == TIM4) {
        /* USER CODE BEGIN TIM4_MspDeInit 0 */

        /* USER CODE END TIM4_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM4_CLK_DISABLE();

        /**TIM4 GPIO Configuration
         PB6     ------> TIM4_CH1
         */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

        /* USER CODE BEGIN TIM4_MspDeInit 1 */

        /* USER CODE END TIM4_MspDeInit 1 */
    } else if (htim_ic->Instance == TIM9) {
        /* USER CODE BEGIN TIM9_MspDeInit 0 */

        /* USER CODE END TIM9_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM9_CLK_DISABLE();

        /**TIM9 GPIO Configuration
         PA2     ------> TIM9_CH1
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);

        /* USER CODE BEGIN TIM9_MspDeInit 1 */

        /* USER CODE END TIM9_MspDeInit 1 */
    } else if (htim_ic->Instance == TIM10) {
        /* USER CODE BEGIN TIM10_MspDeInit 0 */

        /* USER CODE END TIM10_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM10_CLK_DISABLE();

        /**TIM10 GPIO Configuration
         PA6     ------> TIM10_CH1
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);

        /* USER CODE BEGIN TIM10_MspDeInit 1 */

        /* USER CODE END TIM10_MspDeInit 1 */
    } else if (htim_ic->Instance == TIM11) {
        /* USER CODE BEGIN TIM11_MspDeInit 0 */

        /* USER CODE END TIM11_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM11_CLK_DISABLE();

        /**TIM11 GPIO Configuration
         PA7     ------> TIM11_CH1
         */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);

        /* USER CODE BEGIN TIM11_MspDeInit 1 */

        /* USER CODE END TIM11_MspDeInit 1 */
    }
}

void HAL_USART_MspInit(USART_HandleTypeDef *husart) {
    GPIO_InitTypeDef GPIO_InitStruct;
    if (husart->Instance == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();

        HAL_NVIC_SetPriority(USART2_IRQn, ISR_PRIORITY_SERIAL_DBG, 0);

        HAL_NVIC_EnableIRQ(USART2_IRQn);
        __HAL_USART_ENABLE_IT(husart, USART_IT_RXNE);

    } else if (husart->Instance == USART3) {
        __HAL_RCC_USART3_CLK_ENABLE();

        HAL_NVIC_SetPriority(USART3_IRQn, ISR_PRIORITY_SERIAL_MCU, 0);

        HAL_NVIC_EnableIRQ(USART3_IRQn);
        __HAL_USART_ENABLE_IT(husart, USART_IT_RXNE);
    }
}

void HAL_USART_MspDeInit(USART_HandleTypeDef *husart) {
    if (husart->Instance == USART2) {
        __HAL_RCC_USART2_CLK_DISABLE();
    } else if (husart->Instance == USART3) {
        __HAL_RCC_USART3_CLK_DISABLE();
    } else {
        assert(0);
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
 * @
 */

/**
 * @
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
