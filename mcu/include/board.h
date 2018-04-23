/*
 *    Copyright (C) 2015  Crystal-Photonics GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef BOARD_H_
#define BOARD_H_

//#ifndef STM32L151xE
//#define STM32L151xE
//#endif

#include <stdbool.h>
#include <stdint.h>

#if 1
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include <queue.h>
#include <semphr.h>
#endif

#include "chip_init.h"
#include "stm32f4xx_hal.h"

/** Name of the board */
#define BOARD_NAME "rpcFREERTOS_TEMPLATE"

#define rpcFREERTOS_TEMPLATE

// A:
#define ADC_CHAN_1 ADC_CHANNEL_10
#define ADC_CHAN_2 ADC_CHANNEL_1

typedef struct {
    GPIO_InitTypeDef pinDef;
    GPIO_TypeDef *port;
} pinGPIO_t;
/*
typedef struct {
        GPIO_InitTypeDef pinDef;
        GPIO_TypeDef * port;
        uint16_t pinSource;
        uint8_t af;
} pinAlternateFunction_t;
*/

typedef enum { kid_none, kid_key1, kid_onoff, kid_KEYCOUNT } key_id_t;
typedef enum { lid_none, lid_red, lid_green, lid_yellow, lid_LEDCOUNT } led_id_t;
typedef enum { les_none, les_on, les_off } led_state_t;

#define PIN_DISP_D4                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_0, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_DISP_D4() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET)
#define CLEAR_DISP_D4() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET)

#define PIN_DISP_D5                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_1, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_DISP_D5() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET)
#define CLEAR_DISP_D5() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET)

#define PIN_DISP_D6                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_2, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_DISP_D6() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)
#define CLEAR_DISP_D6() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET)

#define PIN_DISP_D7                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_3, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_DISP_D7() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET)
#define CLEAR_DISP_D7() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET)

#define PIN_DISP_RS                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_14, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOE }
#define SET_DISP_RS() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET)
#define CLEAR_DISP_RS() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_RESET)

#define PIN_DISP_E                                                                                                                                   \
    { .pinDef = {.Pin = GPIO_PIN_15, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOE }
#define SET_DISP_E() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET)
#define CLEAR_DISP_E() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET)

#define PIN_GPIO0                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_10, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_GPIO0() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET)
#define CLEAR_GPIO0() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET)

#define PIN_GPIO1                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_11, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_GPIO1() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET)
#define CLEAR_GPIO1() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET)

#define PIN_GPIO2                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_GPIO2() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET)
#define CLEAR_GPIO2() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET)

#define PIN_GPIO3                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_5, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOA }
#define SET_GPIO3() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define CLEAR_GPIO3() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)

#define PIN_GPIO4                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_4, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOE }
#define SET_GPIO4() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET)
#define CLEAR_GPIO4() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET)

#define PIN_WS2812                                                                                                                                   \
    { .pinDef = {.Pin = GPIO_PIN_12, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_WS2812() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET)
#define CLEAR_WS2812() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET)

#define PIN_LED_RED                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOE }
#define SET_LED_RED() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET)
#define CLEAR_LED_RED() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET)

#define PIN_LED_GREEN                                                                                                                                \
    { .pinDef = {.Pin = GPIO_PIN_12, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOE }
#define SET_LED_GREEN() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET)
#define CLEAR_LED_GREEN() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET)

#define PIN_LED_YELLOW                                                                                                                               \
    { .pinDef = {.Pin = GPIO_PIN_11, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOE }
#define SET_LED_YELLOW() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET)
#define CLEAR_LED_YELLOW() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET)

#define PIN_LED_COM_TOP_YELLOW                                                                                                                       \
    { .pinDef = {.Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOC }
#define SET_LED_COM_TOP_YELLOW() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)
#define CLEAR_LED_COM_TOP_YELLOW() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)

#define PIN_LED_COM_TOP_GREEN                                                                                                                        \
    { .pinDef = {.Pin = GPIO_PIN_8, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOA }
#define SET_LED_COM_TOP_GREEN() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET)
#define CLEAR_LED_COM_TOP_GREEN() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET)

#define PIN_LED_COM_BOT_YELLOW                                                                                                                       \
    { .pinDef = {.Pin = GPIO_PIN_14, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_LED_COM_BOT_YELLOW() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET)
#define CLEAR_LED_COM_BOT_YELLOW() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET)

#define PIN_LED_COM_BOT_GREEN                                                                                                                        \
    { .pinDef = {.Pin = GPIO_PIN_6, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOA }
#define SET_LED_COM_BOT_GREEN() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET)
#define CLEAR_LED_COM_BOT_GREEN() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET)

#define PIN_SHUTDOWN                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_10, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOE }
#define SET_SHUTDOWN() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET)
#define CLEAR_SHUTDOWN() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET)

#define PIN_SUPPLY_SENSE                                                                                                                             \
    { .pinDef = {.Pin = GPIO_PIN_1, .Mode = GPIO_MODE_ANALOG, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define GET_SUPPLY_SENSE() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)

#define PIN_ZERO_CROSS                                                                                                                               \
    { .pinDef = {.Pin = GPIO_PIN_9, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_ZERO_CROSS() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9)

#define PIN_SD_DETECT                                                                                                                                \
    { .pinDef = {.Pin = GPIO_PIN_8, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOC }
#define GET_SD_DETECT() HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)

#define PIN_CS_SD                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_7, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOC }
#define SET_CS_SD() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET)
#define CLEAR_CS_SD() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET)

//#define PIN_CS_ADC   			{.pinDef = {.Pin = GPIO_PIN_4, .Mode=GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM,
//.Pull=GPIO_NOPULL }, .port = GPIOA}

#define PIN_CS_ADC_PIO                                                                                                                               \
    { .pinDef = {.Pin = GPIO_PIN_4, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOA }

#define PIN_CS_ADC                                                                                                                                   \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_4, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF6_SPI3},    \
        .port = GPIOA                                                                                                                                \
    }
#define SET_CS_ADC() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define CLEAR_CS_ADC() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

//#define PIN_ADC_SCK   			{.pinDef = {.Pin = GPIO_PIN_10, .Mode=GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM,
//.Pull=GPIO_NOPULL }, .port = GPIOC}

#define PIN_ADC_SCK_PIO                                                                                                                              \
    { .pinDef = {.Pin = GPIO_PIN_10, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOC }

#define PIN_ADC_SCK                                                                                                                                  \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_10, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF6_SPI3},   \
        .port = GPIOC                                                                                                                                \
    }
#define SET_ADC_SCK() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET)
#define CLEAR_ADC_SCK() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET)

//#define PIN_ADC_MISO   			{.pinDef = {.Pin = GPIO_PIN_11, .Mode=GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM,
//.Pull=GPIO_NOPULL
//}, .port = GPIOC}

#define PIN_ADC_MISO_PIO                                                                                                                             \
    {                                                                                                                                                \
        .pinDef =                                                                                                                                    \
            {                                                                                                                                        \
                .Pin = GPIO_PIN_11, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL,                                   \
            },                                                                                                                                       \
        .port = GPIOC                                                                                                                                \
    }

#define PIN_ADC_MISO                                                                                                                                 \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_11,                                                                                                               \
                   .Mode = GPIO_MODE_AF_PP,                                                                                                          \
                   .Speed = GPIO_SPEED_FREQ_MEDIUM,                                                                                                  \
                   .Pull = GPIO_NOPULL,                                                                                                              \
                   .Alternate = GPIO_AF5_I2S3ext},                                                                                                   \
        .port = GPIOC                                                                                                                                \
    }

//#define PIN_ADC_MOSI   			{.pinDef = {.Pin = GPIO_PIN_12, .Mode=GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM,
//.Pull=GPIO_NOPULL }, .port = GPIOC}

#define PIN_ADC_MOSI_PIO                                                                                                                             \
    { .pinDef = {.Pin = GPIO_PIN_12, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOC }

#define PIN_ADC_MOSI                                                                                                                                 \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_12, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF6_SPI3},   \
        .port = GPIOC                                                                                                                                \
    }
#define SET_ADC_MOSI() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET)
#define CLEAR_ADC_MOSI() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET)

#define PIN_KEYONOFF                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_0, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOA }
#define GET_KEYONOFF() HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)

#define PIN_KEY_1                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_5, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_KEY_1() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5)

#define PIN_KEY_2                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_6, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_KEY_2() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6)

#define PIN_KEY_3                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_7, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_KEY_3() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7)

#define PIN_DIP_0                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_0, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_DIP_0() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0)

#define PIN_DIP_1                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_1, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_DIP_1() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_1)

#define PIN_DIP_2                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_2, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_DIP_2() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2)

#define PIN_DIP_3                                                                                                                                    \
    { .pinDef = {.Pin = GPIO_PIN_3, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_DIP_3() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3)

#define PIN_WIND_DIR_GRAY_0                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_8, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOE }
#define GET_WIND_DIR_GRAY_0() HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8)

#define PIN_WIND_DIR_GRAY_1                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_2, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define GET_WIND_DIR_GRAY_1() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)

#define PIN_WIND_DIR_GRAY_2                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_4, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define GET_WIND_DIR_GRAY_2() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)

#define PIN_WIND_DIR_GRAY_3                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_5, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define GET_WIND_DIR_GRAY_3() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5)

#define PIN_WIND_DIR_GRAY_4                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_8, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define GET_WIND_DIR_GRAY_4() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)

#define PIN_WIND_DIR_GRAY_5                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_14, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define GET_WIND_DIR_GRAY_5() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)

#define PIN_WIND_DIR_GRAY_6                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_15, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define GET_WIND_DIR_GRAY_6() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)

#define PIN_WIND_DIR_GRAY_7                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_9, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOC }
#define GET_WIND_DIR_GRAY_7() HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9)

#define PIN_WIND_SPEED_PULS                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_6, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOC }
#define GET_WIND_SPEED_PULS() HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)

#define PIN_WIND_HEATER                                                                                                                              \
    { .pinDef = {.Pin = GPIO_PIN_14, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_WIND_HEATER() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET)
#define CLEAR_WIND_HEATER() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET)

#define PIN_RASP_CS                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_9, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOB }
#define SET_RASP_CS() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
#define CLEAR_RASP_CS() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
#define GET_RASP_CS() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)

#define PIN_RASP_SCK                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_10, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOB }
#define GET_RASP_SCK() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)

#define PIN_RASP_MOSI                                                                                                                                \
    { .pinDef = {.Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOC }
#define GET_RASP_MOSI() HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)

#define PIN_RASP_MISO                                                                                                                                \
    { .pinDef = {.Pin = GPIO_PIN_2, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOC }
#define SET_RASP_MISO() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET)
#define CLEAR_RASP_MISO() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET)

#define PIN_RASP_IRQ                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_0, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define GET_RASP_IRQ() HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0)

#define PIN_RMII_REF_CLK                                                                                                                             \
    { .pinDef = {.Pin = GPIO_PIN_1, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOA }
#define PIN_RMII_MDIO                                                                                                                                \
    { .pinDef = {.Pin = GPIO_PIN_2, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOA }
#define PIN_RMII_CRS                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_7, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOA }
#define PIN_RMII_MDC                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_1, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOC }
#define PIN_RMII_TX_EN                                                                                                                               \
    { .pinDef = {.Pin = GPIO_PIN_11, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define PIN_RMII_TX0                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_12, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define PIN_RMII_TX1                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_13, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOB }
#define PIN_RMII_RX0                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_4, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOC }
#define PIN_RMII_RX1                                                                                                                                 \
    { .pinDef = {.Pin = GPIO_PIN_5, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOC }

#define PIN_DISCO_LED_RED                                                                                                                            \
    { .pinDef = {.Pin = GPIO_PIN_14, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_DISCO_LED_RED() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET)
#define CLEAR_DISCO_LED_RED() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET)

#define PIN_DISCO_LED_GREEN                                                                                                                          \
    { .pinDef = {.Pin = GPIO_PIN_12, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_DISCO_LED_GREEN() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET)
#define CLEAR_DISCO_LED_GREEN() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET)

#define PIN_MCU_TX_ENABLE                                                                                                                            \
    { .pinDef = {.Pin = GPIO_PIN_7, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOD }
#define SET_MCU_TX_ENABLE() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET)
#define CLEAR_MCU_TX_ENABLE() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET)

#define PIN_MCU_RX                                                                                                                                   \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_9, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF7_USART3},    \
        .port = GPIOD                                                                                                                                \
    }
#define PIN_MCU_TX                                                                                                                                   \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_8, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF7_USART3},    \
        .port = GPIOD                                                                                                                                \
    }

#define PIN_DBG_RX                                                                                                                                   \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_6, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF7_USART2},    \
        .port = GPIOD                                                                                                                                \
    }
#define PIN_DBG_TX                                                                                                                                   \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_5, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_HIGH, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF7_USART2},    \
        .port = GPIOD                                                                                                                                \
    }

//#define PIN_DISCO_DBG_RX 				{.pinDef = {.Pin = GPIO_PIN_3, .Mode=GPIO_MODE_AF_PP, .Speed= GPIO_SPEED_FREQ_HIGH,
//.Pull=GPIO_NOPULL  , .Alternate= GPIO_AF7_USART2}, .port = GPIOA}
//#define PIN_DISCO_DBG_TX 				{.pinDef = {.Pin = GPIO_PIN_2 , .Mode=GPIO_MODE_AF_PP, .Speed= GPIO_SPEED_FREQ_HIGH,
//.Pull=GPIO_NOPULL , .Alternate= GPIO_AF7_USART2 },  .port = GPIOA}

#define PIN_PIO_DBG_TX                                                                                                                               \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_5, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_HIGH, .Pull = GPIO_PULLUP, .Alternate = GPIO_AF4_I2C2},      \
        .port = GPIOD                                                                                                                                \
    }
#define SET_PIO_DBG_TX() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define CLEAR_PIO_DBG_TX() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)

#define PIN_MCU_SDA                                                                                                                                  \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_7, .Mode = GPIO_MODE_AF_OD, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF4_I2C1},    \
        .port = GPIOB                                                                                                                                \
    }
#define PIN_MCU_SCL                                                                                                                                  \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_6, .Mode = GPIO_MODE_AF_OD, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF4_I2C1},    \
        .port = GPIOB                                                                                                                                \
    }

#define PIN_PIO_SDA                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_7, .Mode = GPIO_MODE_OUTPUT_OD, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOB }
#define PIN_PIO_SCL                                                                                                                                  \
    { .pinDef = {.Pin = GPIO_PIN_6, .Mode = GPIO_MODE_OUTPUT_OD, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOB }

#define PIN_USB_N                                                                                                                                    \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_11,                                                                                                               \
                   .Mode = GPIO_MODE_AF_PP,                                                                                                          \
                   .Speed = GPIO_SPEED_FREQ_MEDIUM,                                                                                                  \
                   .Pull = GPIO_NOPULL,                                                                                                              \
                   .Alternate = GPIO_AF10_OTG_FS},                                                                                                   \
        .port = GPIOA                                                                                                                                \
    }
#define PIN_USB_P                                                                                                                                    \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_12,                                                                                                               \
                   .Mode = GPIO_MODE_AF_PP,                                                                                                          \
                   .Speed = GPIO_SPEED_FREQ_MEDIUM,                                                                                                  \
                   .Pull = GPIO_NOPULL,                                                                                                              \
                   .Alternate = GPIO_AF10_OTG_FS},                                                                                                   \
        .port = GPIOA                                                                                                                                \
    }
#define PIN_USB_ID                                                                                                                                   \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_10,                                                                                                               \
                   .Mode = GPIO_MODE_AF_PP,                                                                                                          \
                   .Speed = GPIO_SPEED_FREQ_MEDIUM,                                                                                                  \
                   .Pull = GPIO_NOPULL,                                                                                                              \
                   .Alternate = GPIO_AF10_OTG_FS},                                                                                                   \
        .port = GPIOA                                                                                                                                \
    }
#define PIN_USB_VBUS                                                                                                                                 \
    {                                                                                                                                                \
        .pinDef = {.Pin = GPIO_PIN_9, .Mode = GPIO_MODE_AF_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL, .Alternate = GPIO_AF10_OTG_FS}, \
        .port = GPIOA                                                                                                                                \
    }

#define PIN_USB_POWER_ON                                                                                                                             \
    { .pinDef = {.Pin = GPIO_PIN_0, .Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOC }
#define SET_USB_POWER_ON() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET)
#define CLEAR_USB_POWER_ON() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET)

#define PIN_USB_POWER_FAIL                                                                                                                           \
    { .pinDef = {.Pin = GPIO_PIN_4, .Mode = GPIO_MODE_INPUT, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_PULLDOWN}, .port = GPIOD }
#define GET_USB_POWER_FAIL() HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4)

#define PIN_ADC_CURR_EXT                                                                                                                             \
    { .pinDef = {.Pin = GPIO_PIN_3, .Mode = GPIO_MODE_ANALOG, .Speed = GPIO_SPEED_FREQ_MEDIUM, .Pull = GPIO_NOPULL}, .port = GPIOA }

#define COM_DBG_BASE USART2
//#define COM_DBG_CLK RCC_APB2Periph_USART2

#define COM_RPC_BASE USART3
//#define COM_RPC_CLK RCC_APB2Periph_USART3

#define I2S3 SPI3
#define I2S3_CLK_ENABLE() __HAL_RCC_SPI3_CLK_ENABLE()
#define I2S3_CLK_DISABLE() __HAL_RCC_SPI3_CLK_DISABLE()

typedef enum {
    ext_adc_channel_raw_curr_l1_pos = 0,
    ext_adc_channel_raw_curr_l1_neg = 1,
    ext_adc_channel_raw_curr_l2_pos = 2,
    ext_adc_channel_raw_curr_l2_neg = 3,
    ext_adc_channel_raw_curr_l3_pos = 4,
    ext_adc_channel_raw_curr_l3_neg = 5,

    ext_adc_channel_raw_vref = 6,
    ext_adc_channel_raw_volt_l12 = 7,
    ext_adc_channel_raw_volt_l23 = 8,
    ext_adc_channel_raw_volt_l31 = 9,
    ext_adc_channel_raw_aux_volt = 10,

    ext_adc_channel_raw_temp_l1 = 12,
    ext_adc_channel_raw_temp_l2 = 13,
    ext_adc_channel_raw_temp_l3 = 14,

    ext_adc_channel_raw_gnd1 = 15,
    ext_adc_channel_raw_gnd2 = 11,
} ext_adc_raw_channel_t;

typedef enum {
    ext_adc_value_curr_l1,
    ext_adc_value_curr_l2,
    ext_adc_value_curr_l3,

    ext_adc_value_vref,

    ext_adc_value_volt_l12,
    ext_adc_value_volt_l23,
    ext_adc_value_volt_l31,

    ext_adc_value_aux_volt,

    ext_adc_value_temp_l1,
    ext_adc_value_temp_l2,
    ext_adc_value_temp_l3,
    ext_adc_value_COUNT
} ext_adc_value_channel_t;

void boardConfigurePIO(void);
void boardPowerOnPIO(void);
void boardInitAFPin(const pinGPIO_t *pinAF);
void boardInitPin(const pinGPIO_t *pin);

void boardSetPinsToIntput(void);
void boardReinitPins(void);

bool boardTestKey(key_id_t keyID);
void boardSetLED(led_id_t ledID, led_state_t ledState);
void boardDisplayWriteData(uint8_t data);

#endif /* BOARD_H_ */
