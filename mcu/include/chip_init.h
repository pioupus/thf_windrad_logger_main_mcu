/*
 * chip_init.h
 *
 *  Created on: 20.04.2016
 *      Author: ak
 */

#ifndef MCU_INCLUDE_CHIP_INIT_H_
#define MCU_INCLUDE_CHIP_INIT_H_
#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc;

// extern DAC_HandleTypeDef hdac;

extern I2C_HandleTypeDef hi2c2;

extern I2S_HandleTypeDef hi2s3;
extern I2S_HandleTypeDef hi2s3_ext; //

extern TIM_HandleTypeDef htim4;

extern USART_HandleTypeDef husart2;
extern USART_HandleTypeDef husart3;

extern TIM_HandleTypeDef htim2;

void SystemClock_Config(void);
void MX_RTC_Init(void);
void MX_ADC_Init(void);
#endif /* MCU_INCLUDE_CHIP_INIT_H_ */
