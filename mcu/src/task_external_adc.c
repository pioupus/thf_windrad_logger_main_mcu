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

#include "task_external_adc.h"
#include "main.h"
#include "task.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

const uint16_t ADS_MANUAL_MODE = 0x1000;
const uint16_t ADS_NXT_CHANNEL_POS = 7;

const uint16_t ADS_GPIO_MODE = 0x000F;

const uint16_t ADS_CTR_REG = 0x4000;
const uint16_t ADS_CTR_REG_RESET = 0x0200;
const uint16_t ADS_CTR_REG_GPIO_OUTPUT = 0x000F;

#define CHANNEL_COUNT 14
#define ADC_WORD_SIZE 1

#define RX_BUFFER_SIZE ADC_RECORDS_TO_BUFFER *CHANNEL_COUNT *ADC_WORD_SIZE / 8
#define TX_BUFFER_SIZE ADC_RECORDS_TO_BUFFER *CHANNEL_COUNT *ADC_WORD_SIZE / 8

uint32_t adc_input_buffer[RX_BUFFER_SIZE];
uint32_t adc_output_buffer[TX_BUFFER_SIZE];

typedef struct {
    int16_t value;
} adc_data_record_t;

adc_data_record_t adc_channels[ext_adc_value_COUNT][ADC_RECORDS_TO_BUFFER];
uint16_t timestamps[ADC_RECORDS_TO_BUFFER];

#define COMPLETE_RECORDS_CHUNK_COUNT 4 * 32
#define COMPLETE_RECORDS_LENGTH ADC_RECORDS_TO_BUFFER *COMPLETE_RECORDS_CHUNK_COUNT
adc_data_record_t adc_channels_complete[ext_adc_value_COUNT][COMPLETE_RECORDS_LENGTH];

static int16_t power_currents_average[i_COUNT];
static uint16_t power_currents_effective[i_COUNT];
static int16_t power_currents_max[i_COUNT];

static int16_t power_voltages_average[u_COUNT];
static uint16_t power_voltages_effective[u_COUNT];
static uint16_t power_voltages_max[u_COUNT];
static int32_t power_power = 0;
static int32_t period_length = 0;

static uint16_t temparatures_average[3];

static uint32_t complete_records_chunk_write_index = 0;
static uint32_t complete_records_chunk_read_index = 0;
static volatile bool half_full_handle_activated = false;
static uint16_t read_buffer_start_index = 0;

const ext_adc_raw_channel_t ORDER_OF_ACQUISITION[] = {
#if 1
    ext_adc_channel_raw_curr_l1_neg, ext_adc_channel_raw_curr_l1_pos, ext_adc_channel_raw_curr_l2_pos, ext_adc_channel_raw_curr_l2_neg,
    ext_adc_channel_raw_curr_l3_pos, ext_adc_channel_raw_curr_l3_neg, ext_adc_channel_raw_vref,        ext_adc_channel_raw_volt_l21,
    ext_adc_channel_raw_volt_l23,    ext_adc_channel_raw_volt_l13,    ext_adc_channel_raw_temp_l1,     ext_adc_channel_raw_temp_l2,
    ext_adc_channel_raw_temp_l3,     ext_adc_channel_raw_aux_volt
#endif
};

#define LAST_INDEX_IN_ACQUIRE_ORDER (sizeof(ORDER_OF_ACQUISITION)) / (sizeof(ext_adc_raw_channel_t)) - 1

#define LAST_CHANNEL_IN_ACQUIRE_ORDER ORDER_OF_ACQUISITION[LAST_INDEX_IN_ACQUIRE_ORDER]

static SemaphoreHandle_t semaphore_adc_half_buffer_ready;

static void I2S3_ClockConfig_(RCC_PeriphCLKInitTypeDef *rccclkinit) {

    /*Enable PLLI2S clock*/
    HAL_RCCEx_GetPeriphCLKConfig(rccclkinit);
    /* PLLI2S_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */

    /* Audio frequency multiple of 8 (8/16/32/48/96/192)*/
    /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN = 192 Mhz */
    /* I2SCLK = PLLI2S_VCO Output/PLLI2SR = 192/6 = 32 Mhz */
    rccclkinit->PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit->PLLI2S.PLLI2SN = 256;
    rccclkinit->PLLI2S.PLLI2SR = 1;
}

static void I2S3_ClockConfig(uint32_t AudioFreq) {
    RCC_PeriphCLKInitTypeDef rccclkinit = {0};
    I2S3_ClockConfig_(&rccclkinit);
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
}

static uint64_t get_i2s3_clock(uint32_t AudioFreq) {
    RCC_PeriphCLKInitTypeDef rccclkinit;
    I2S3_ClockConfig_(&rccclkinit);
    // volatile uint32_t uwTimclock = HAL_RCC_GetPCLK1Freq();
    // HAL_RCC_GetPCLK1Freq: 42Mhz

    // HAL_RCC_GetPCLK2Freq: 84 Mhz
    uint32_t i2sclk;
#if defined(I2S_APB1_APB2_FEATURE)
    if (IS_I2S_APB1_INSTANCE(hi2s->Instance)) {
        i2sclk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2S_APB1);
    } else {
        i2sclk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2S_APB2);
    }
#else
    i2sclk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I2S);
#endif

    return i2sclk;
    // HAL_RCC_GetPCLK2Freq();
}

static void I2S3_MspInit(void) {
    static DMA_HandleTypeDef hdma_i2sTx;
    static DMA_HandleTypeDef hdma_i2sRx;

    /* Enable I2S3 clock */

    /* Enable the I2S DMA clock */
    __HAL_RCC_DMA1_CLK_ENABLE();

    // if (hi2s3.Instance == I2S3) {

    /* Configure the hdma_i2sTx handle parameters */
    hdma_i2sTx.Init.Channel = DMA_CHANNEL_0;
    hdma_i2sTx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2sTx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2sTx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2sTx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_i2sTx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_i2sTx.Init.Mode = DMA_CIRCULAR;
    hdma_i2sTx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_i2sTx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    hdma_i2sTx.Instance = DMA1_Stream5;

    /* Associate the DMA handle */

    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_i2sTx);
    __HAL_LINKDMA(&hi2s3, hdmatx, hdma_i2sTx);

    hdma_i2sRx.Init.Channel = DMA_CHANNEL_2;
    hdma_i2sRx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_i2sRx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2sRx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2sRx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_i2sRx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_i2sRx.Init.Mode = DMA_CIRCULAR;
    hdma_i2sRx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma_i2sRx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    hdma_i2sRx.Instance = DMA1_Stream2;

    /* Configure the DMA Stream */
    HAL_DMA_Init(&hdma_i2sRx);
    __HAL_LINKDMA(&hi2s3, hdmarx, hdma_i2sRx);

    /* I2S DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, ISR_PRIORITY_EXT_ADC_TX, 0);
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, ISR_PRIORITY_EXT_ADC_RX, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
}

static void MX_I2S_Init_DMA(void) {

    hi2s3.Instance = I2S3;
    __HAL_I2S_DISABLE(&hi2s3);

    hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
    hi2s3.Init.Standard = I2S_STANDARD_PCM_SHORT;
    hi2s3.Init.DataFormat = I2S_DATAFORMAT_32B;
    hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    hi2s3.Init.AudioFreq = 700000U;
    // 600000U: 9.85Mhz

    // 7000 00U: 11.64MHz
    // I2S_AUDIOFREQ_192K; // 500000U;
    hi2s3.Init.CPOL = I2S_CPOL_LOW;

    hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
    hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;

    __HAL_I2S_ENABLE(&hi2s3);
    HAL_I2S_Init(&hi2s3);

    I2S3_MspInit();
}

static void MX_I2S_Init_poll(void) {

    hi2s3.Instance = I2S3;
    __HAL_I2S_DISABLE(&hi2s3);

    hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
    hi2s3.Init.Standard = I2S_STANDARD_PCM_SHORT;
    hi2s3.Init.DataFormat = I2S_DATAFORMAT_32B;
    hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_192K;
    hi2s3.Init.CPOL = I2S_CPOL_LOW;
    hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
    hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;

    __HAL_I2S_ENABLE(&hi2s3);
    HAL_I2S_Init(&hi2s3);

    I2S3_MspInit();
}

static void ExternalADC_Config(void) {

    semaphore_adc_half_buffer_ready = xSemaphoreCreateBinary();
}

void extadc_get_voltages_max(uint16_t max[u_COUNT]) {
    portENTER_CRITICAL();
    for (int i = 0; i < u_COUNT; i++) {
        max[i] = power_voltages_max[i];
        power_voltages_max[i] = 0;
    }
    portEXIT_CRITICAL();
}

void extadc_get_voltages_avg(int16_t avg[u_COUNT]) {
    portENTER_CRITICAL();
    for (int i = 0; i < u_COUNT; i++) {
        avg[i] = power_voltages_average[i];
    }
    portEXIT_CRITICAL();
}

void extadc_get_voltages_effective(uint16_t eff[3]) {
    portENTER_CRITICAL();
    for (int i = 0; i < 3; i++) {
        eff[i] = power_voltages_effective[i];
    }
    portEXIT_CRITICAL();
}

void extadc_get_currents_avg(int16_t avg[i_COUNT]) {
    portENTER_CRITICAL();
    for (int i = 0; i < i_COUNT; i++) {
        avg[i] = power_currents_average[i];
    }
    portEXIT_CRITICAL();
}

void extadc_get_currents_effective(uint16_t eff[i_COUNT]) {
    portENTER_CRITICAL();
    for (int i = 0; i < i_COUNT; i++) {
        eff[i] = power_currents_effective[i];
    }
    portEXIT_CRITICAL();
}

void extadc_get_currents_max(uint16_t max[i_COUNT]) {
    portENTER_CRITICAL();
    for (int i = 0; i < i_COUNT; i++) {
        max[i] = power_currents_max[i];
        power_currents_max[i] = 0;
    }
    portEXIT_CRITICAL();
}

void extadc_get_temperature_avg(uint16_t avg[3]) {
    portENTER_CRITICAL();
    for (int i = 0; i < i_COUNT; i++) {
        avg[i] = temparatures_average[i];
    }
    portEXIT_CRITICAL();
}

uint16_t extadc_get_frequency() {
    uint16_t result = 0;
    const uint8_t spi_sample_lenght = 32;
    uint64_t spi_frequency = get_i2s3_clock(192);
    // hi2s->Instance->I2SPR = (uint32_t)((uint32_t)i2sdiv | (uint32_t)(i2sodd | (uint32_t)hi2s->Init.MCLKOutput));
    uint32_t I2SDIV = (hi2s3.Instance->I2SPR & SPI_I2SPR_I2SDIV_Msk) >> SPI_I2SPR_I2SDIV_Pos;
    uint32_t odd = (hi2s3.Instance->I2SPR & SPI_I2SPR_ODD_Msk) >> SPI_I2SPR_ODD_Pos;
    uint32_t i2s_bit_clock = spi_frequency / (((2 * I2SDIV) + odd) * 2); // when the channel frame is 32-bit wide
    uint32_t i2s_sample_clock = i2s_bit_clock / (32 * CHANNEL_COUNT);

    uint64_t sample_time_us_over_10 = 100 * 1000 * 1000 / i2s_sample_clock;

    // CHANNEL_COUNT
    uint32_t period_length_ = 0;
    portENTER_CRITICAL();
    period_length_ = period_length;
    portEXIT_CRITICAL();
    uint64_t period_length_us_over_10 = 0;
    if (period_length_) {
        period_length_us_over_10 = period_length_ * sample_time_us_over_10;
    } else {
        period_length_us_over_10 = 0;
    }
    uint64_t frequency = 100 * 1000 * 1000 / period_length_us_over_10;
    result = frequency;
    return result;
}

int32_t extadc_get_power() {
    int32_t result = 0;
    portENTER_CRITICAL();
    result = power_power;
    portEXIT_CRITICAL();
    return result;
}

uint16_t extadc_sample_chunk_count() {
    return COMPLETE_RECORDS_CHUNK_COUNT;
}

void extadc_reset_sample_data_readpointer() {
    complete_records_chunk_read_index = 0;
}

void extadc_get_sample_data(int16_t samples[ADC_RECORDS_TO_BUFFER], const ext_adc_value_channel_t channel) {
    portENTER_CRITICAL();
    for (uint i = 0; i < ADC_RECORDS_TO_BUFFER; i++) {
        samples[i] = adc_channels_complete[channel][i + complete_records_chunk_read_index * ADC_RECORDS_TO_BUFFER].value;
    }
    complete_records_chunk_read_index++;
    if (complete_records_chunk_read_index >= COMPLETE_RECORDS_CHUNK_COUNT) {
        complete_records_chunk_read_index = 0;
    }
    portEXIT_CRITICAL();
}

void extadc_start_acquire_sample_data() {
    complete_records_chunk_write_index = 0;
}

bool extadc_is_sample_data_complete() {
    return complete_records_chunk_write_index >= COMPLETE_RECORDS_CHUNK_COUNT;
}

uint8_t external_adc_transmission() {

    uint8_t next_channel_index = 0;
    assert(sizeof(ORDER_OF_ACQUISITION) == CHANNEL_COUNT);
#if 1
    MX_I2S_Init_poll();
    adc_output_buffer[0] = ADS_CTR_REG | ADS_CTR_REG_RESET | ADS_CTR_REG_GPIO_OUTPUT;
    adc_output_buffer[1] = 0;
    HAL_I2S_Transmit(&hi2s3, (uint16_t *)adc_output_buffer, 1, 100); // reset ADS Chip
    vTaskDelay((50 / portTICK_RATE_MS));
#endif
    MX_I2S_Init_DMA();

    for (int i = 0; i < TX_BUFFER_SIZE; i++) {
        adc_output_buffer[i] = ADS_MANUAL_MODE | (ORDER_OF_ACQUISITION[next_channel_index] << ADS_NXT_CHANNEL_POS);
        adc_input_buffer[i] = 0;
        next_channel_index++;
        if (next_channel_index >= CHANNEL_COUNT) {
            next_channel_index = 0;
        }
    }
    HAL_I2SEx_TransmitReceive_DMA(&hi2s3, (uint16_t *)adc_output_buffer, (uint16_t *)&adc_input_buffer[0], RX_BUFFER_SIZE);

    return 0;
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
    __asm__("BKPT");
}

void HAL_I2S_TxRxCpltCallback(I2S_HandleTypeDef *hi2s) {
    SET_LED_GREEN();
    CLEAR_LED_GREEN();
    __asm__("BKPT");
}

void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s) {
    SET_LED_GREEN();
    CLEAR_LED_GREEN();
    __asm__("BKPT");
}

void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (DMA1_Stream2_IRQHandler_triggered) {
        half_full_handle_activated = true;
        // HAL_I2SEx_TxRxHalfCpltCallback_(hi2s);
    }
}

void HAL_I2SEx_TxRxHalfCpltCallback_(I2S_HandleTypeDef *hi2s) {
    /* Prevent unused argument(s) compilation warning */
    if (DMA1_Stream2_IRQHandler_triggered) {
        static portBASE_TYPE xHigherPriorityTaskWoken;
        volatile uint32_t ndtr = hi2s->hdmarx->Instance->NDTR;
        // if (ndtr < RX_BUFFER_SIZE / 2) {
        // read_buffer_start_index = 0;
        //} else {
        ////    read_buffer_start_index = RX_BUFFER_SIZE / 2;
        // }
        half_full_handle_activated = false;
        SET_LED_GREEN();
#if 0
    {
        static uint8_t led_green_state = 0;
        if (led_green_state & 1) {
            SET_LED_GREEN();
        } else {
            CLEAR_LED_GREEN();
        }
        led_green_state++;
    }
#endif

        xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(semaphore_adc_half_buffer_ready, &xHigherPriorityTaskWoken);
        CLEAR_LED_GREEN();
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

int32_t apply_calibration(int32_t in_value, const calibration_t *calibration, const ext_adc_value_channel_t channel) {
    int64_t result = 0;
    if (in_value > 0) {
        result = calib_apply_calibration(in_value, &calibration->channel_pos[channel]);
    } else {
        result = calib_apply_calibration(in_value, &calibration->channel_neg[channel]);
    }
    return result;
}

uint16_t abs_i16(int16_t val) {
    if (val < 0) {
        return -val;
    } else {
        return val;
    }
}

void taskExternalADC(void *pvParameters) {

    for (uint i = 0; i < LAST_INDEX_IN_ACQUIRE_ORDER + 1; i++) { // check if there are duplicates. If yes, the data processing beneath wont work
        for (uint n = i + 1; n < LAST_INDEX_IN_ACQUIRE_ORDER + 1; n++) {
            assert(ORDER_OF_ACQUISITION[i] != ORDER_OF_ACQUISITION[n]);
        }
    }

    calibration_t *calibration_data = calib_get();
    ExternalADC_Config();
    I2S3_ClockConfig(192);
    external_adc_transmission();
    // xSemaphoreTake(semaphore_adc_half_buffer_ready, 100 / portTICK_RATE_MS);

    uint32_t write_index = 0;
    uint32_t sample_time_stamp = 0;
    int64_t temperatures_avging[3] = {0};
    int64_t power_avging = {0};
    int64_t currents_avging[i_COUNT] = {0};
    int64_t voltages_avging[u_COUNT] = {0};
    int64_t period_length_avging = 0;

    uint64_t currents_effectiving[i_COUNT] = {0};
    uint64_t voltages_effectiving[u_COUNT] = {0};

    uint16_t currents_max_abs[i_COUNT] = {0};
    uint32_t voltages_max_abs[u_COUNT] = {0};
    int32_t value_volt_l21_old_max_abs = 0;

    uint64_t effectiving_test_possible_overflow = {0};

    bool period_length_histeresis_rising = true;
    uint32_t period_avg_cycles_n = 0;
    uint32_t period_length_last_transition_phase = 0;
    uint32_t period_length_counter = 0;

    uint32_t printf_pause = 0;
    uint32_t effectiving_n = 0;
    TickType_t effective_calculation_period_start_ms = xTaskGetTickCount();
    const TickType_t EFFECTIVE_CALCULATION_PERIOD_LENGTH_ms = 1000 / portTICK_RATE_MS;

    adc_data_record_t data_record[16] = {{0}};

    for (;;) {
        if (xSemaphoreTake(semaphore_adc_half_buffer_ready, 100 / portTICK_RATE_MS) == pdTRUE) {
            SET_LED_YELLOW();

            for (uint16_t i = 0; i < RX_BUFFER_SIZE / 2; i++) {

                uint32_t adc_word = adc_input_buffer[read_buffer_start_index + i];
                ext_adc_raw_channel_t channel_index = (adc_word & 0xF000) >> 12;
#if 0
                {
                    if (printf_pause % 100 == 0) {
                        printf("%d\n", channel_index);
                    }
                }
#endif
                uint16_t value = adc_word & 0x0FFF;

#if 1
                assert(channel_index < 16);
                data_record[channel_index].value = value;
                (void)data_record;
#endif

#if 1
                const ext_adc_raw_channel_t LAST_CHANNEL_IN_ORDER = LAST_CHANNEL_IN_ACQUIRE_ORDER;
                if (channel_index == LAST_CHANNEL_IN_ACQUIRE_ORDER) {
                    // if we acquired a hole set of data and start with the first channel again
                    timestamps[write_index] = sample_time_stamp;
#if 0
                    {
                        static uint8_t led_green_state = 0;
                        if (led_green_state & 1) {
                            SET_LED_GREEN();
                        } else {
                            CLEAR_LED_GREEN();
                        }
                        led_green_state++;
                    }
#endif

                    assert(write_index < ADC_RECORDS_TO_BUFFER);
                    { //
                        const ext_adc_value_channel_t ci = ext_adc_value_curr_l1;
                        if (data_record[ext_adc_channel_raw_curr_l1_pos].value > data_record[ext_adc_channel_raw_curr_l1_neg].value) {
                            adc_channels[ci][write_index].value = data_record[ext_adc_channel_raw_curr_l1_pos].value;

                        } else {
                            adc_channels[ci][write_index].value = -data_record[ext_adc_channel_raw_curr_l1_neg].value;
                        }
                    }

                    { //
                        const ext_adc_value_channel_t ci = ext_adc_value_curr_l2;
                        if (data_record[ext_adc_channel_raw_curr_l2_pos].value > data_record[ext_adc_channel_raw_curr_l2_neg].value) {
                            adc_channels[ci][write_index].value = data_record[ext_adc_channel_raw_curr_l2_pos].value;

                        } else {
                            adc_channels[ci][write_index].value = -data_record[ext_adc_channel_raw_curr_l2_neg].value;
                        }
                    }
                    { //
                        const ext_adc_value_channel_t ci = ext_adc_value_curr_l3;
                        if (data_record[ext_adc_channel_raw_curr_l3_pos].value > data_record[ext_adc_channel_raw_curr_l3_neg].value) {
                            adc_channels[ci][write_index].value = data_record[ext_adc_channel_raw_curr_l3_pos].value;

                        } else {
                            adc_channels[ci][write_index].value = -data_record[ext_adc_channel_raw_curr_l3_neg].value;
                        }
                    }

                    {
                        adc_channels[ext_adc_value_volt_l21][write_index].value =
                            data_record[ext_adc_channel_raw_volt_l21].value - data_record[ext_adc_channel_raw_vref].value;

                        adc_channels[ext_adc_value_volt_l32][write_index].value =
                            data_record[ext_adc_channel_raw_volt_l23].value - data_record[ext_adc_channel_raw_vref].value;

                        adc_channels[ext_adc_value_volt_l13][write_index].value =
                            data_record[ext_adc_channel_raw_volt_l13].value - data_record[ext_adc_channel_raw_vref].value;
                    }

                    {
                        adc_channels[ext_adc_value_aux_volt][write_index].value = data_record[ext_adc_channel_raw_aux_volt].value;
                        adc_channels[ext_adc_value_temp_l1][write_index].value = data_record[ext_adc_channel_raw_temp_l1].value;
                        adc_channels[ext_adc_value_temp_l2][write_index].value = data_record[ext_adc_channel_raw_temp_l2].value;
                        adc_channels[ext_adc_value_temp_l3][write_index].value = data_record[ext_adc_channel_raw_temp_l3].value;

                        adc_channels[ext_adc_value_vref][write_index].value = data_record[ext_adc_channel_raw_vref].value;
                    }
#if 1
                    for (int i = 0; i < ext_adc_value_COUNT; i++) {
                        adc_channels[i][write_index].value = apply_calibration(adc_channels[i][write_index].value, calibration_data, i);
                    }

                    period_length_counter++;
                    if (period_length_histeresis_rising == true) {
                        if (adc_channels[ext_adc_value_volt_l21][write_index].value < -value_volt_l21_old_max_abs / 2) {
                            period_length_histeresis_rising = false;
                            period_length_avging += period_length_counter;
                            period_length_counter = 0;
                            period_avg_cycles_n++;
                        }
                    } else {
                        if (adc_channels[ext_adc_value_volt_l21][write_index].value > value_volt_l21_old_max_abs / 2) {
                            period_length_histeresis_rising = true;
                        }
                    }

                    // https://de.wikipedia.org/wiki/Aronschaltung
                    int64_t p3 = adc_channels[ext_adc_value_volt_l32][write_index].value * adc_channels[ext_adc_value_curr_l3][write_index].value;
                    int64_t p2 = -adc_channels[ext_adc_value_volt_l21][write_index].value * adc_channels[ext_adc_value_curr_l1][write_index].value;
                    power_avging += (p3 + p2);

                    temperatures_avging[0] += adc_channels[ext_adc_value_temp_l1][write_index].value;
                    temperatures_avging[1] += adc_channels[ext_adc_value_temp_l2][write_index].value;
                    temperatures_avging[2] += adc_channels[ext_adc_value_temp_l3][write_index].value;

                    currents_avging[i_l1] += adc_channels[ext_adc_value_curr_l1][write_index].value;
                    currents_avging[i_l2] += adc_channels[ext_adc_value_curr_l2][write_index].value;
                    currents_avging[i_l3] += adc_channels[ext_adc_value_curr_l3][write_index].value;

                    if (currents_max_abs[i_l1] < abs_i16(adc_channels[ext_adc_value_curr_l1][write_index].value)) {
                        currents_max_abs[i_l1] = abs_i16(adc_channels[ext_adc_value_curr_l1][write_index].value);
                    }

                    if (currents_max_abs[i_l2] < abs_i16(adc_channels[ext_adc_value_curr_l2][write_index].value)) {
                        currents_max_abs[i_l2] = abs_i16(adc_channels[ext_adc_value_curr_l2][write_index].value);
                    }

                    if (currents_max_abs[i_l3] < abs_i16(adc_channels[ext_adc_value_curr_l3][write_index].value)) {
                        currents_max_abs[i_l3] = abs_i16(adc_channels[ext_adc_value_curr_l3][write_index].value);
                    }

                    if (voltages_max_abs[u_l21] < abs_i16(adc_channels[ext_adc_value_volt_l21][write_index].value)) {
                        voltages_max_abs[u_l21] = abs_i16(adc_channels[ext_adc_value_volt_l21][write_index].value);
                    }

                    if (voltages_max_abs[u_l32] < abs_i16(adc_channels[ext_adc_value_volt_l32][write_index].value)) {
                        voltages_max_abs[u_l32] = abs_i16(adc_channels[ext_adc_value_volt_l32][write_index].value);
                    }
                    if (voltages_max_abs[u_l13] < abs_i16(adc_channels[ext_adc_value_volt_l13][write_index].value)) {
                        voltages_max_abs[u_l13] = abs_i16(adc_channels[ext_adc_value_volt_l13][write_index].value);
                    }
                    if (voltages_max_abs[u_aux] < abs_i16(adc_channels[ext_adc_value_aux_volt][write_index].value)) {
                        voltages_max_abs[u_aux] = abs_i16(adc_channels[ext_adc_value_aux_volt][write_index].value);
                    }

                    voltages_avging[u_l21] += adc_channels[ext_adc_value_volt_l21][write_index].value;
                    voltages_avging[u_l32] += adc_channels[ext_adc_value_volt_l32][write_index].value;
                    voltages_avging[u_l13] += adc_channels[ext_adc_value_volt_l13][write_index].value;
                    voltages_avging[u_aux] += adc_channels[ext_adc_value_aux_volt][write_index].value;

                    currents_effectiving[i_l1] +=
                        adc_channels[ext_adc_value_curr_l1][write_index].value * adc_channels[ext_adc_value_curr_l1][write_index].value;
                    currents_effectiving[i_l2] +=
                        adc_channels[ext_adc_value_curr_l2][write_index].value * adc_channels[ext_adc_value_curr_l2][write_index].value;
                    currents_effectiving[i_l3] +=
                        adc_channels[ext_adc_value_curr_l3][write_index].value * adc_channels[ext_adc_value_curr_l3][write_index].value;

                    voltages_effectiving[u_l21] +=
                        adc_channels[ext_adc_value_volt_l21][write_index].value * adc_channels[ext_adc_value_volt_l21][write_index].value;
                    voltages_effectiving[u_l32] +=
                        adc_channels[ext_adc_value_volt_l32][write_index].value * adc_channels[ext_adc_value_volt_l32][write_index].value;
                    voltages_effectiving[u_l13] +=
                        adc_channels[ext_adc_value_volt_l13][write_index].value * adc_channels[ext_adc_value_volt_l13][write_index].value;

                    effectiving_test_possible_overflow += 4096 * 4096;

                    assert(effectiving_test_possible_overflow < ((uint64_t)1 << 63));

                    effectiving_n++;
                    if (effective_calculation_period_start_ms + EFFECTIVE_CALCULATION_PERIOD_LENGTH_ms < xTaskGetTickCount()) {
                        assert(effectiving_n);
                        for (int i = 0; i < 3; i++) {
                            currents_effectiving[i] /= effectiving_n;
                            power_currents_effective[i] = round(sqrt(currents_effectiving[i]));
                            currents_effectiving[i] = 0;
                        }
                        for (int i = 0; i < 3; i++) {
                            voltages_effectiving[i] /= effectiving_n;
                            power_voltages_effective[i] = round(sqrt(voltages_effectiving[i]));
                            voltages_effectiving[i] = 0;
                        }

                        for (int i = 0; i < 3; i++) {
                            temparatures_average[i] = temperatures_avging[i] / effectiving_n;
                            temperatures_avging[i] = 0;
                        }

                        for (int i = 0; i < i_COUNT; i++) {
                            power_currents_average[i] = currents_avging[i] / effectiving_n;
                            currents_avging[i] = 0;
                        }

                        for (int i = 0; i < u_COUNT; i++) {
                            power_voltages_average[i] = voltages_avging[i] / effectiving_n;
                            voltages_avging[i] = 0;
                        }

                        for (int i = 0; i < i_COUNT; i++) {
                            if (currents_max_abs[i] > power_currents_max[i]) {
                                power_currents_max[i] = currents_max_abs[i];
                            }
                            currents_max_abs[i] = 0;
                        }
                        value_volt_l21_old_max_abs = voltages_max_abs[u_l21];

                        for (int i = 0; i < u_COUNT; i++) {
                            if (voltages_max_abs[i] > power_voltages_max[i]) {
                                power_voltages_max[i] = voltages_max_abs[i];
                            }
                            voltages_max_abs[i] = 0;
                        }
                        period_length = period_length_avging / period_avg_cycles_n;
                        period_length_avging = 0;
                        period_avg_cycles_n = 0;
                        power_power = power_avging / effectiving_n;
                        power_avging = 0;
                        effectiving_test_possible_overflow = 0;
                        //__asm__("BKPT");
                        effective_calculation_period_start_ms = xTaskGetTickCount();
                        effectiving_n = 0;
                    }

#if 1
                    write_index++;
                    if (write_index >= ADC_RECORDS_TO_BUFFER) {
                        write_index = 0;
                        portENTER_CRITICAL();
#if 1
                        if (complete_records_chunk_write_index < COMPLETE_RECORDS_CHUNK_COUNT) {
                            for (int j = 0; j < ext_adc_value_COUNT; j++) {
                                for (int i = 0; i < ADC_RECORDS_TO_BUFFER; i++) {
                                    adc_channels_complete[j][(complete_records_chunk_write_index * ADC_RECORDS_TO_BUFFER) + i] = adc_channels[j][i];
                                }
                            }
                            //  memcpy(&adc_channels_complete[complete_records_chunk_write_index * ADC_RECORDS_TO_BUFFER], adc_channels,
                            //        sizeof(adc_channels));
                            complete_records_chunk_write_index++;
                        }
#endif
                        portEXIT_CRITICAL();
                    }
#endif
#endif
                }
#endif
                sample_time_stamp++;
            }

            read_buffer_start_index += RX_BUFFER_SIZE / 2;
            if (read_buffer_start_index >= RX_BUFFER_SIZE) {
                read_buffer_start_index = 0;
                //       printf_pause++;
            }
            CLEAR_LED_YELLOW();
        }
    }
}
