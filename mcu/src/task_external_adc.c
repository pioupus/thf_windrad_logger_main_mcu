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

#define RX_BUFFER_SIZE 256
#define TX_BUFFER_SIZE 256
uint16_t adc_input_buffer[RX_BUFFER_SIZE];
uint16_t adc_output_buffer[TX_BUFFER_SIZE];

void I2S3_ClockConfig(I2S_HandleTypeDef *hi2s, uint32_t AudioFreq) {
    RCC_PeriphCLKInitTypeDef rccclkinit;

    /*Enable PLLI2S clock*/
    HAL_RCCEx_GetPeriphCLKConfig(&rccclkinit);
    /* PLLI2S_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */

    /* Audio frequency multiple of 8 (8/16/32/48/96/192)*/
    /* PLLI2S_VCO Output = PLLI2S_VCO Input * PLLI2SN = 192 Mhz */
    /* I2SCLK = PLLI2S_VCO Output/PLLI2SR = 192/6 = 32 Mhz */
    rccclkinit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    rccclkinit.PLLI2S.PLLI2SN = 256;
    rccclkinit.PLLI2S.PLLI2SR = 1;
    HAL_RCCEx_PeriphCLKConfig(&rccclkinit);
}

/**
  * @brief  Starts playing audio stream from a data buffer for a determined size.
  * @param  pBuffer: Pointer to the buffer
  * @param  Size: Number of audio data BYTES.
  * @retval AUDIO_OK if correct communication, else wrong communication
  */
uint8_t external_adc_transmission() {
    /* Call the audio Codec Play function */

    /* Update the Media layer and enable it for play */
    //  HAL_I2S_Transmit_DMA(&adc_output_buffer, pBuffer, DMA_MAX(Size / 2));

    //  memset(adc_output_buffer, 0xAA, RX_BUFFER_SIZE);
    for (int i = 0; i < TX_BUFFER_SIZE; i++) {
        adc_output_buffer[i] = i;
        adc_input_buffer[i] = i;
    }
    HAL_I2SEx_TransmitReceive_DMA(&hi2s3, (uint16_t *)adc_output_buffer, (uint16_t *)&adc_input_buffer[0], RX_BUFFER_SIZE);
    // HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t *)adc_output_buffer, RX_BUFFER_SIZE);
    // HAL_I2S_Receive_DMA(&hi2s3, (uint16_t *)adc_output_buffer, RX_BUFFER_SIZE);

    /* Return AUDIO_OK when all operations are correctly done */
    return 0;
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

#if 1
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
//  }
#endif
    /* I2S DMA IRQ Channel configuration */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, ISR_PRIORITY_EXT_ADC_TX, 0);
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, ISR_PRIORITY_EXT_ADC_RX, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
}

#if 1
/* RTC init function */
void MX_I2S_Init(void) {

    hi2s3.Instance = I2S3;
    __HAL_I2S_DISABLE(&hi2s3);

    hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
    hi2s3.Init.Standard = I2S_STANDARD_PCM_SHORT;
    hi2s3.Init.DataFormat = I2S_DATAFORMAT_32B;
    hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_192K;
    hi2s3.Init.CPOL = I2S_CPOL_HIGH;
    hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
    hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;

    __HAL_I2S_ENABLE(&hi2s3);
    HAL_I2S_Init(&hi2s3);

#if 0
    hi2s3_ext.Instance = I2S3ext;
    __HAL_I2S_DISABLE(&hi2s3_ext);

    hi2s3_ext.Init.Mode = I2S_MODE_SLAVE_RX;
    hi2s3_ext.Init.Standard = I2S_STANDARD_PCM_SHORT;
    hi2s3_ext.Init.DataFormat = I2S_DATAFORMAT_32B;
    hi2s3_ext.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    hi2s3_ext.Init.AudioFreq = I2S_AUDIOFREQ_192K;
    hi2s3_ext.Init.CPOL = I2S_CPOL_HIGH;
    hi2s3_ext.Init.ClockSource = I2S_CLOCK_PLL;
    // hi2s3_ext.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;

    __HAL_I2S_ENABLE(&hi2s3_ext);
    HAL_I2S_Init(&hi2s3_ext);

#endif

    I2S3_MspInit();
    //__HAL_I2S_ENABLE_IT(&hi2s3, I2S_IT_TXE);
    //__HAL_I2S_ENABLE_IT(&hi2s3_ext, I2S_IT_RXNE);

    /// I2S3ext
}
#else

/**
  * @brief  Initializes the Audio Codec audio interface (I2S)
  * @note   This function assumes that the I2S input clock
  *         is already configured and ready to be used.
  * @param  AudioFreq: Audio frequency to be configured for the I2S peripheral.
  */
static void MX_I2S_Init(uint32_t AudioFreq) {
    /* Initialize the hAudioInI2s and haudio_in_i2sext Instance parameters */
    hi2s3.Instance = AUDIO_IN_I2Sx;
    hi2s3_ext.Instance = I2S3ext;

    /* Disable I2S block */
    __HAL_I2S_DISABLE(&hi2s3);
    __HAL_I2S_DISABLE(&haudio_in_i2sext);

    /* I2S peripheral configuration */
    hi2s3.Init.AudioFreq = AudioFreq;
    hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
    hi2s3.Init.CPOL = I2S_CPOL_LOW;
    hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
    hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
    hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
    hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
    /* Init the I2S */
    HAL_I2S_Init(&hi2s3);

#if 0
    /* I2Sext peripheral configuration */
    hi2s3_ext.Init.AudioFreq = AudioFreq;
    hi2s3_ext.Init.ClockSource = I2S_CLOCK_PLL;
    hi2s3_ext.Init.CPOL = I2S_CPOL_HIGH;
    hi2s3_ext.Init.DataFormat = I2S_DATAFORMAT_16B;
    hi2s3_ext.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hi2s3_ext.Init.Mode = I2S_MODE_SLAVE_RX;
    hi2s3_ext.Init.Standard = I2S_STANDARD_PHILIPS;

    /* Init the I2Sext */
    HAL_I2S_Init(&hi2s3_ext);

    /* Enable I2S block */
    __HAL_I2S_ENABLE(&hi2s3_ext);
#endif
    __HAL_I2S_ENABLE(&hi2s3);
}
#endif

static SemaphoreHandle_t semaphoreExternalADCReady;

/**
  * @brief  Configures the ADC1 channel5.
  * @param  None
  * @retval None
  */
void ExternalADC_Config(void) {

    semaphoreExternalADCReady = xSemaphoreCreateBinary();
}

#if 0
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    __asm__("BKPT");
    external_adc_transmission();
}
#endif

#if 0
/**
 * @brief HAL Handler for Codec DMA Interrupt
 */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    // external_adc_transmission();
    __asm__("BKPT");
}
#endif
#if 0
/**
  * @brief  Tx Transfer completed callbacks.
  * @param  hi2s: I2S handle
  */
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    //  if (hi2s->Instance == I2S3) {
    /* Call the user function which will manage directly transfer complete */
    // BSP_AUDIO_OUT_TransferComplete_CallBack();
    __asm__("BKPT");
    external_adc_transmission();
    //  }
}
#endif
#if 0
/**
  * @brief  Tx Half Transfer completed callbacks.
  * @param  hi2s: I2S handle
  */
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    // if (hi2s->Instance == I2S3) {
    /* Manage the remaining file size and new address offset: This function should
       be coded by user (its prototype is already declared in stm32f4_discovery_audio.h) */
    //  BSP_AUDIO_OUT_HalfTransfer_CallBack();
    //  }
    __asm__("BKPT");
}
#endif
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
    //  if (hi2s->Instance == I2S3) {
    /* Manage the remaining file size and new address offset: This function should
       be coded by user (its prototype is already declared in stm32f4_discovery_audio.h) */
    //  BSP_AUDIO_OUT_HalfTransfer_CallBack();
    //  }
    __asm__("BKPT");
}

/**
  * @brief  Rx Transfer completed callbacks
  * @param  hi2s: I2S handle
  */
#if 0
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    /* Call the record update function to get the next buffer to fill and its size (size is ignored) */
    // if (hi2s->Instance == I2S3) {
    /* Call the user function which will manage directly transfer complete */
    // BSP_AUDIO_OUT_TransferComplete_CallBack();
    external_adc_transmission();
    // }
}
#endif

void HAL_I2S_TxRxCpltCallback(I2S_HandleTypeDef *hi2s) {
    /* Call the record update function to get the next buffer to fill and its size (size is ignored) */
    // if (hi2s->Instance == I2S3) {
    /* Call the user function which will manage directly transfer complete */
    // BSP_AUDIO_OUT_TransferComplete_CallBack();
    // external_adc_transmission();
    // __asm__("BKPT");
    // }
}

void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2s);
    // __asm__("BKPT");
    /* NOTE : This function Should not be modified, when the callback is needed,
              the HAL_I2SEx_TxRxHalfCpltCallback could be implemented in the user file
     */
}

#if 0
/**
  * @brief  Rx Half Transfer completed callbacks.
  * @param  hi2s: I2S handle
  */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    /* Manage the remaining file size and new address offset: This function
       should be coded by user (its prototype is already declared in stm32f4_discovery_audio.h) */
    // if (hi2s->Instance == I2S3) {
    /* Call the user function which will manage directly transfer complete */
    // BSP_AUDIO_OUT_TransferComplete_CallBack();
    // }
}
#endif

void taskExternalADC(void *pvParameters) {

    MX_I2S_Init();
    I2S3_ClockConfig(&hi2s3, 192);
    // for (int i = 0; i < adsi_max - 1; i++) {
    //}
    external_adc_transmission();
    for (;;) {
        vTaskDelay(100);
    }
}
