/*
    FreeRTOS V8.2.1 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
        BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.

        ***Note*** This example uses queues to send each character into an interrupt
        service routine and out of an interrupt service routine individually.  This
        is done to demonstrate queues being used in an interrupt, and to deliberately
        load the system to test the FreeRTOS port.  It is *NOT* meant to be an
        example of an efficient implementation.  An efficient implementation should
        use FIFO's or DMA if available, and only use FreeRTOS API functions when
        enough has been received to warrant a task being unblocked to process the
        data.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "board.h"
#include "main.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */

/* Demo application includes. */
#include "serial.h"
/*-----------------------------------------------------------*/

/* Misc defines. */
#define serINVALID_QUEUE ((QueueHandle_t)0)
#define serNO_BLOCK ((TickType_t)0)

/*-----------------------------------------------------------*/

static bool RTOSRunningFlag = false;
/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars_dbg;
static QueueHandle_t xCharsForTx_dbg;

static QueueHandle_t xRxedChars_rpc;
static QueueHandle_t xCharsForTx_rpc;

/*-----------------------------------------------------------*/

/*
 * See the serial2.h header file.
 */
void xSerialPortInitMinimal(eCOMPort pxPort, unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength) {

    if (pxPort == serCOM_DBG) {
        /* Create the queues used to hold Rx/Tx characters. */
        xRxedChars_dbg = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE)sizeof(signed char));
        xCharsForTx_dbg = xQueueCreate(uxQueueLength + 1, (unsigned portBASE_TYPE)sizeof(signed char));

        /* If the queues were created correctly then setup the serial port
        hardware. */
        if ((xRxedChars_dbg != serINVALID_QUEUE) && (xCharsForTx_dbg != serINVALID_QUEUE)) {

            husart2.Instance = COM_DBG_BASE;
            husart2.Init.BaudRate = ulWantedBaud;
            husart2.Init.WordLength = USART_WORDLENGTH_8B;
            husart2.Init.StopBits = USART_STOPBITS_1;
            husart2.Init.Parity = USART_PARITY_NONE;
            husart2.Init.Mode = USART_MODE_TX;
            // husart2.Init.HwFlowCtl = USART_HWCONTROL_NONE;
            // husart2.Init.OverSampling = USART_OVERSAMPLING_8;

            HAL_USART_Init(&husart2);
            CLEAR_BIT(husart2.Instance->CR2, USART_CR2_CLKEN);
        }
    } else if (pxPort == serCOM_RPC) {
        /* Create the queues used to hold Rx/Tx characters. */
        xRxedChars_rpc = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE)sizeof(signed char));
        xCharsForTx_rpc = xQueueCreate(uxQueueLength + 1, (unsigned portBASE_TYPE)sizeof(signed char));

        /* If the queues were created correctly then setup the serial port
        hardware. */
        if ((xRxedChars_rpc != serINVALID_QUEUE) && (xCharsForTx_rpc != serINVALID_QUEUE)) {

            husart3.Instance = COM_RPC_BASE;
            husart3.Init.BaudRate = ulWantedBaud;
            husart3.Init.WordLength = USART_WORDLENGTH_8B;
            husart3.Init.StopBits = USART_STOPBITS_1;
            husart3.Init.Parity = USART_PARITY_NONE;
            husart3.Init.Mode = USART_MODE_TX_RX;
            husart3.Init.CLKPolarity = USART_POLARITY_LOW;
            husart3.Init.CLKPhase = USART_PHASE_1EDGE;
            husart3.Init.CLKLastBit = USART_MODE_TX_RX;

            HAL_USART_Init(&husart3);
            CLEAR_BIT(husart3.Instance->CR2, USART_CR2_CLKEN);
        }
        /* This demo file only supports a single port but we have to return
        something to comply with the standard demo header file. */
    }
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar(eCOMPort pxPort, signed char *pcRxedChar, TickType_t xBlockTime) {
    if (pxPort == serCOM_DBG) {
        /* Get the next character from the buffer.  Return false if no characters
        are available, or arrive before xBlockTime expires. */
        if (xQueueReceive(xRxedChars_dbg, pcRxedChar, xBlockTime)) {
            return pdTRUE;
        } else {
            return pdFALSE;
        }
    } else if (pxPort == serCOM_RPC) {
        /* Get the next character from the buffer.  Return false if no characters
        are available, or arrive before xBlockTime expires. */
        if (xQueueReceive(xRxedChars_rpc, pcRxedChar, xBlockTime)) {
            return pdTRUE;
        } else {
            return pdFALSE;
        }
    }
    return pdFALSE;
}
/*-----------------------------------------------------------*/
void serialSetRTOSRunningFlag(bool rtosRunningFlag) {
    RTOSRunningFlag = rtosRunningFlag;
}

void vSerialPutString(eCOMPort pxPort, const char *pcString, unsigned short usStringLength) {
    signed char *pxNext;

    /* Send each character in the string, one at a time. */
    pxNext = (signed char *)pcString;
    for (int i = 0; i < usStringLength; i++) {
        xSerialPutChar(pxPort, *pxNext, serNO_BLOCK);
        pxNext++;
    }
}
/*-----------------------------------------------------------*/

void xSerialPutCharPolling(eCOMPort pxPort, signed char cOutChar) {
    if (pxPort == serCOM_DBG) {
        HAL_USART_Transmit(&husart2, (uint8_t *)&cOutChar, 1, HAL_MAX_DELAY);
    } else if (pxPort == serCOM_RPC) {
        HAL_USART_Transmit(&husart3, (uint8_t *)&cOutChar, 1, HAL_MAX_DELAY);
    }
}

signed portBASE_TYPE xSerialPutChar(eCOMPort pxPort, signed char cOutChar, TickType_t xBlockTime) {
    signed portBASE_TYPE xReturn = pdFAIL;

    if (RTOSRunningFlag) {
        if (pxPort == serCOM_DBG) {
            if (xQueueSend(xCharsForTx_dbg, &cOutChar, xBlockTime) == pdPASS) {
                xReturn = pdPASS;
                __HAL_USART_ENABLE_IT(&husart2, USART_IT_TXE);

            } else {
                xReturn = pdFAIL;
            }
        } else if (pxPort == serCOM_RPC) {
            if (xQueueSend(xCharsForTx_rpc, &cOutChar, xBlockTime) == pdPASS) {
                xReturn = pdPASS;
                __HAL_USART_ENABLE_IT(&husart3, USART_IT_TXE);

            } else {
                xReturn = pdFAIL;
            }
        }

    } else {
        xSerialPutCharPolling(pxPort, cOutChar);
        xReturn = pdPASS;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose(eCOMPort xPort) { /* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

void USART2_IRQHandler(void) {
    uint32_t tmp_flag = 0, tmp_it_source = 0;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    char cChar;

    tmp_flag = __HAL_USART_GET_FLAG(&husart2, USART_FLAG_TXE);
    tmp_it_source = __HAL_USART_GET_IT_SOURCE(&husart2, USART_IT_TXE);
    /* UART parity error interrupt occurred ------------------------------------*/
    if ((tmp_flag != RESET) && (tmp_it_source != RESET)) {
        /* The interrupt was caused by the TX register becoming empty.  Are
                        there any more characters to transmit? */
        if (xQueueReceiveFromISR(xCharsForTx_dbg, &cChar, &xHigherPriorityTaskWoken) == pdTRUE) {
            /* A character was retrieved from the queue so can be sent to the
                            USART now. */
            husart2.Instance->DR = cChar;
        } else {
            __HAL_USART_DISABLE_IT(&husart2, USART_IT_TXE);
        }
    }

    tmp_flag = __HAL_USART_GET_FLAG(&husart2, USART_FLAG_RXNE);
    tmp_it_source = __HAL_USART_GET_IT_SOURCE(&husart2, USART_IT_RXNE);
    /* UART in mode Receiver ---------------------------------------------------*/
    if ((tmp_flag != RESET) && (tmp_it_source != RESET)) {
        /* A character has been received on the USART, send it to the Rx
        handler task. */
        cChar = husart2.Instance->DR;
        xQueueSendFromISR(xRxedChars_dbg, &cChar, &xHigherPriorityTaskWoken);
    }

    /* If sending or receiving from a queue has caused a task to unblock, and
    the unblocked task has a priority equal to or higher than the currently
    running task (the task this ISR interrupted), then xHigherPriorityTaskWoken
    will have automatically been set to pdTRUE within the queue send or receive
    function.  portEND_SWITCHING_ISR() will then ensure that this ISR returns
    directly to the higher priority unblocked task. */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void USART3_IRQHandler(void) {
    uint32_t tmp_flag = 0, tmp_it_source = 0;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    char cChar;

    tmp_flag = __HAL_USART_GET_FLAG(&husart3, USART_FLAG_TXE);
    tmp_it_source = __HAL_USART_GET_IT_SOURCE(&husart3, USART_IT_TXE);
    /* UART parity error interrupt occurred ------------------------------------*/
    if ((tmp_flag != RESET) && (tmp_it_source != RESET)) {
        /* The interrupt was caused by the TX register becoming empty.  Are
                        there any more characters to transmit? */
        if (xQueueReceiveFromISR(xCharsForTx_rpc, &cChar, &xHigherPriorityTaskWoken) == pdTRUE) {
            /* A character was retrieved from the queue so can be sent to the
                            USART now. */
            husart3.Instance->DR = cChar;
        } else {
            __HAL_USART_DISABLE_IT(&husart3, USART_IT_TXE);
            CLEAR_LED_COM_BOT_YELLOW();
            CLEAR_LED_COM_TOP_YELLOW();

            CLEAR_LED_COM_BOT_GREEN();
            CLEAR_LED_COM_TOP_GREEN();
        }
    }

    tmp_flag = __HAL_USART_GET_FLAG(&husart3, USART_FLAG_RXNE);
    tmp_it_source = __HAL_USART_GET_IT_SOURCE(&husart3, USART_IT_RXNE);
    /* UART in mode Receiver ---------------------------------------------------*/
    if ((tmp_flag != RESET) && (tmp_it_source != RESET)) {
        /* A character has been received on the USART, send it to the Rx
        handler task. */
        cChar = husart3.Instance->DR;
        xQueueSendFromISR(xRxedChars_rpc, &cChar, &xHigherPriorityTaskWoken);
    }

    /* If sending or receiving from a queue has caused a task to unblock, and
    the unblocked task has a priority equal to or higher than the currently
    running task (the task this ISR interrupted), then xHigherPriorityTaskWoken
    will have automatically been set to pdTRUE within the queue send or receive
    function.  portEND_SWITCHING_ISR() will then ensure that this ISR returns
    directly to the higher priority unblocked task. */
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
