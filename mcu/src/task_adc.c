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

#include "board.h"
#include "task.h"
#include "main.h"
#include "rpc_transmission/client/generated_app/RPC_TRANSMISSION_mcu2qt.h"
#include "task_adc.h"
#include "storage_calibration.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "channel_codec/channel_codec.h"

#define FACTORY_TSCALIB_MD_BASE ((uint32_t)0x1FF80078)   /*!< Calibration Data Bytes base address for medium density devices*/
#define FACTORY_TSCALIB_MDP_BASE ((uint32_t)0x1FF800F8)  /*!< Calibration Data Bytes base address for medium density plus devices*/
#define FACTORY_TSCALIB_F407_VGT6 ((uint32_t)0x1FFF7A2A) /*!< Calibration Data Bytes base address for medium density plus devices*/
#define FACTORY_TSCALIB_MD_DATA ((TSCALIB_TypeDef *)FACTORY_TSCALIB_MD_BASE)
#define FACTORY_TSCALIB_MDP_DATA ((TSCALIB_TypeDef *)FACTORY_TSCALIB_MDP_BASE)
#define FACTORY_TSCALIB_F407_VGT6_DATA ((TSCALIB_F407VGT6_TypeDef *)FACTORY_TSCALIB_F407_VGT6)

#define TEST_CALIB_DIFF                                                                                                                              \
    (int32_t)50 /* difference of hot-cold calib                                                                                                      \
 data to be considered as valid */

#define VBAT_FACTOR_407VGT6 2

/*
STM32L151RET6:
VREFINT_CAL 	0x1FF8 00F8 - 0x1FF8 00F9 	Raw data acquired at temperature of 30C �5 C VDDA= 3 V �10 mV
TSCAL1 			0x1FF8 00FA - 0x1FF8 00FB	TS ADC raw data acquired at temperature of 30 �C �5 �C VDDA= 3 V �10 mV
TSCAL2			0x1FF8 00FE - 0x1FF8 00FF	TS ADC raw data acquired at temperature of 110 �C �5 �C VDDA= 3 V �10 mV
*/

/*
 * STM32F407VGT6:
VREFIN_CAL Raw data acquired at temperature of 30 °C, VDDA=3.3 V 0x1FFF7A2A - 0x1FFF7A2B
TS_CAL1 TS ADC raw data acquired at temperature of 30 °C, VDDA=3.3 V 0x1FFF7A2C - 0x1FFF7A2D
TS_CAL2 TS ADC raw data acquired at temperature of 110 °C, VDDA=3.3 V 0x1FFF7A2E - 0x1FFF7A2F
*/
typedef struct {
    uint16_t VREF;
    uint16_t TS_CAL_1; // low temperature calibration data
    uint16_t reserved;
    uint16_t TS_CAL_2; // high temperature calibration data
} TSCALIB_TypeDef;

typedef struct {
    uint16_t VREF;
    uint16_t TS_CAL_1; // low temperature calibration data
    uint16_t TS_CAL_2; // high temperature calibration data
} TSCALIB_F407VGT6_TypeDef;

typedef struct {
    uint16_t VREF;
    uint16_t TS_CAL_1; // low temperature calibration data
    uint16_t TS_CAL_2; // high temperature calibration data
} TSCALIB_t;

typedef struct {
    adc_sequence_index_t index;
    uint16_t value;
} conversion_result_t;

static TSCALIB_t adcCalibData;
static QueueHandle_t adcConversionsQueue;

uint32_t ubat_avgsum;
uint16_t adcValuesPlain[adsi_max];
uint16_t adcValues_smoothed[adsi_max];

static uint16_t acquired_adc_values = 0;
static const uint16_t VALUES_NEEDED_FOR_BEING_VALID = 16;

const uint32_t adcCHANNELS[] = {ADC_CHANNEL_TEMPSENSOR, ADC_CHAN_CURR_EXT, ADC_CHAN_SUPPLY_SENSE, ADC_CHANNEL_VBAT, ADC_CHANNEL_VREFINT};

volatile adc_sequence_index_t adcSequenceIndex;

void adc_get_values(uint16_t values[adsi_max]) {
    portENTER_CRITICAL();
    memcpy(values, adcValues_smoothed, sizeof(adcValues_smoothed));

    calibration_t *calibration = calib_get();

    assert(adsi_temperature < ADC_MCU_VALUE_COUNT);
    values[adsi_temperature] = calib_apply_calibration(values[adsi_temperature], &calibration->cpu_channels[adsi_temperature]);

    assert(adsi_curr_ext < ADC_MCU_VALUE_COUNT);
    values[adsi_curr_ext] = calib_apply_calibration(values[adsi_curr_ext], &calibration->cpu_channels[adsi_curr_ext]);

    assert(adsi_supply_sensse < ADC_MCU_VALUE_COUNT);
    values[adsi_supply_sensse] = calib_apply_calibration(values[adsi_supply_sensse], &calibration->cpu_channels[adsi_supply_sensse]);

    portEXIT_CRITICAL();
}

static void getFactoryTSCalibData(TSCALIB_t *data) {
    uint32_t deviceID;

    deviceID = HAL_GetDEVID();

    if (deviceID == 0x427) {
        TSCALIB_TypeDef data_;
        data_ = *FACTORY_TSCALIB_MDP_DATA;
        data->TS_CAL_1 = data_.TS_CAL_1;
        data->TS_CAL_2 = data_.TS_CAL_2;
        data->VREF = data_.VREF;
    } else if (deviceID == 0x437) {
        TSCALIB_TypeDef data_;
        data_ = *FACTORY_TSCALIB_MDP_DATA;
        data->TS_CAL_1 = data_.TS_CAL_1;
        data->TS_CAL_2 = data_.TS_CAL_2;
        data->VREF = data_.VREF;

    } else if (deviceID == 0x416) {
        TSCALIB_TypeDef data_;
        data_ = *FACTORY_TSCALIB_MD_DATA;
        data->TS_CAL_1 = data_.TS_CAL_1;
        data->TS_CAL_2 = data_.TS_CAL_2;
        data->VREF = data_.VREF;

    } else if (deviceID == 0x413) {

        TSCALIB_F407VGT6_TypeDef data_;
        data_ = *FACTORY_TSCALIB_F407_VGT6_DATA;
        data->TS_CAL_1 = data_.TS_CAL_1;
        data->TS_CAL_2 = data_.TS_CAL_2;
        data->VREF = data_.VREF;
    } else {
        printf("Error loading ADC Calib Data\n");
        assert(0);
        while (1)
            ; // add error handler - device cannot be identified calibration data not loaded!
    }
}

ErrorStatus testFactoryCalibData(void) {
    int32_t testdiff;
    ErrorStatus retval = ERROR;
    TSCALIB_t datatotest;
    getFactoryTSCalibData(&datatotest);
    testdiff = datatotest.TS_CAL_2 - datatotest.TS_CAL_1;
    if (testdiff > TEST_CALIB_DIFF)
        retval = SUCCESS;
    return retval;
}

static void startConversion(adc_sequence_index_t channelindex) {
    // if (adc_running == false) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = adcCHANNELS[channelindex];
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
    HAL_ADC_ConfigChannel(&hadc, &sConfig);
    adcSequenceIndex = channelindex;
    //  adc_running = true;
    HAL_ADC_Start_IT(&hadc);
}

/**
 * @brief  Configures the ADC1 channel5.
 * @param  None
 * @retval None
 */
void ADC_Config(void) {

    ADC_ChannelConfTypeDef sConfig;

    if (testFactoryCalibData() == SUCCESS) {
        getFactoryTSCalibData(&adcCalibData);
    }

    hadc.Instance = ADC1;
    hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode = ENABLE;
    hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.NbrOfConversion = 1;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.DMAContinuousRequests = DISABLE;

    HAL_ADC_Init(&hadc);

    for (int i = 0; i < adsi_max; i++) {
        sConfig.Channel = adcCHANNELS[i];
        sConfig.Rank = i + 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
        HAL_ADC_ConfigChannel(&hadc, &sConfig);
    }
}

bool adc_values_valid() {
    return VALUES_NEEDED_FOR_BEING_VALID < acquired_adc_values;
}

void taskADC(void *pvParameters) {

    const uint16_t SMOOTHVALUE = 16;
    ADC_Config();

    adcConversionsQueue = xQueueCreate(10, sizeof(conversion_result_t));

    uint32_t adcValues_smoothed_SMOOTHVALUE[adsi_max];

    for (int i = 0; i < adsi_max; i++) {
        adcValues_smoothed_SMOOTHVALUE[i] = 0;
    }

    startConversion(0);

    for (;;) {
        conversion_result_t conversion_result = {0};
        if (xQueueReceive(adcConversionsQueue, &conversion_result, 500 / portTICK_RATE_MS) == pdTRUE) {
            //        if (xSemaphoreTake(semaphoreADCReady, 1000 / portTICK_RATE_MS) == pdTRUE) {

            adcValuesPlain[conversion_result.index] = conversion_result.value;
            if (conversion_result.index == adsi_ref) {
                const int32_t VOLT_REFERENCE_mv = 3300;
                const int32_t ADC_MAX_VALUE_DIGIT = 4095;

                uint32_t ref_voltage_mv = VOLT_REFERENCE_mv * adcCalibData.VREF;
                ref_voltage_mv /= adcValuesPlain[adsi_ref];

                int32_t temperature_c = (int32_t)adcValuesPlain[adsi_temperature] * ref_voltage_mv;
                temperature_c /= VOLT_REFERENCE_mv;

                temperature_c = (110 - 30) * (temperature_c - adcCalibData.TS_CAL_1);
                temperature_c /= (adcCalibData.TS_CAL_2 - adcCalibData.TS_CAL_1);
                temperature_c += 30;

                uint32_t adc1_mv = ref_voltage_mv * adcValuesPlain[adsi_curr_ext];
                adc1_mv /= ADC_MAX_VALUE_DIGIT;

                uint32_t adc2_mv = ref_voltage_mv * adcValuesPlain[adsi_supply_sensse];
                adc2_mv /= ADC_MAX_VALUE_DIGIT;

                uint32_t coin_cell_mv = ref_voltage_mv * adcValuesPlain[adsi_coin_cell] * VBAT_FACTOR_407VGT6;
                coin_cell_mv /= ADC_MAX_VALUE_DIGIT;

                adcValues_smoothed_SMOOTHVALUE[adsi_temperature] += temperature_c;
                adcValues_smoothed_SMOOTHVALUE[adsi_ref] += ref_voltage_mv;
                adcValues_smoothed_SMOOTHVALUE[adsi_curr_ext] += adc1_mv;
                adcValues_smoothed_SMOOTHVALUE[adsi_supply_sensse] += adc2_mv;
                adcValues_smoothed_SMOOTHVALUE[adsi_coin_cell] += coin_cell_mv;

                for (int i = 0; i < adsi_max; i++) {
                    adcValues_smoothed[i] = adcValues_smoothed_SMOOTHVALUE[i] / SMOOTHVALUE;
                    adcValues_smoothed_SMOOTHVALUE[i] -= adcValues_smoothed[i];
                }
                if (!adc_values_valid()) {
                    acquired_adc_values++;
                }
                vTaskDelay((100 / portTICK_RATE_MS));
                conversion_result.index = 0;
            } else {
                conversion_result.index++;
            }
            startConversion(conversion_result.index);
        }
    }
}

static portBASE_TYPE xHigherPriorityTaskWoken_ByADC_interrupt = pdFALSE;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
#if 0
    	xHigherPriorityTaskWoken_ByADC_interrupt = pdFALSE;
        adcValuesPlain[adcSequenceIndex] = HAL_ADC_GetValue(hadc);
        adcSequenceIndex++;
        if (adcSequenceIndex >= adsi_max) {
            xSemaphoreGiveFromISR(semaphoreADCReady, &xHigherPriorityTaskWoken_ByADC_interrupt);
            adcSequenceIndex = 1;
        } else {
            hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
        }
#else

    conversion_result_t conversion_result = {0};

    conversion_result.value = HAL_ADC_GetValue(hadc);
    conversion_result.index = adcSequenceIndex;

    xQueueSendFromISR(adcConversionsQueue, &conversion_result, &xHigherPriorityTaskWoken_ByADC_interrupt);
#endif
}

/**
 * @brief  This function handles ADC1 global interrupts requests.
 * @param  None
 * @retval None
 */
void ADC1_IRQHandler(void) {
    HAL_ADC_IRQHandler(&hadc);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken_ByADC_interrupt);

#if 0
    if (__HAL_ADC_GET_IT_SOURCE(&hadc, ADC_IT_EOC)) {
        if (__HAL_ADC_GET_FLAG(&hadc, ADC_FLAG_EOC)) {
            portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

            adcValuesPlain[adcSequenceIndex] = HAL_ADC_GetValue(&hadc);
            adcSequenceIndex++;
            if (adcSequenceIndex >= adsi_max) {
                xSemaphoreGiveFromISR(semaphoreADCReady, &xHigherPriorityTaskWoken);
                adcSequenceIndex = 1;
            }
            __HAL_ADC_CLEAR_FLAG(&hadc, ADC_FLAG_STRT | ADC_FLAG_EOC);
            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
    }
#endif
}
