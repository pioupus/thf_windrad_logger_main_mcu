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

#ifndef MAIN_H_
#define MAIN_H_

#include "board.h"
#include "channel_codec/channel_codec_types.h"
#include "semphr.h"

#define mainLED_TASK_STACK ((unsigned short)550)
#define mainKEY_TASK_STACK ((unsigned short)300)
#define mainRPC_SERIAL_TASK_STACK ((unsigned short)550)
#define mainADC_TASK_STACK ((unsigned short)300)
#define mainEXTERNAL_ADC_TASK_STACK ((unsigned short)300)
#define mainDISPLAY_TASK_STACK ((unsigned short)300)
#define mainSUPERVISOR_TASK_STACK ((unsigned short)300)

#define mainRPC_TASK_SERIAL_PRIORITY tskIDLE_PRIORITY + 1
#define mainLED_TASK_PRIORITY tskIDLE_PRIORITY + 4
#define mainKEY_TASK_PRIORITY tskIDLE_PRIORITY + 3
#define mainADC_TASK_PRIORITY tskIDLE_PRIORITY + 3
#define mainSUPERVISOR_PRIORITY tskIDLE_PRIORITY + 3
#define mainEXTERNAL_ADC_TASK_PRIORITY tskIDLE_PRIORITY + 3
#define mainDISPLAY_TASK_PRIORITY tskIDLE_PRIORITY + 3

#define ISR_PRIORITY_RTC configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3
#define ISR_PRIORITY_SERIAL_DBG configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2
#define ISR_PRIORITY_SERIAL_MCU configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2
#define ISR_PRIORITY_PIO configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 0
#define ISR_PRIORITY_COUNTER configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 0
#define ISR_PRIORITY_ADC configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 0
#define ISR_PRIORITY_EXT_ADC_TX configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 0
#define ISR_PRIORITY_EXT_ADC_RX configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 0
#define ISR_PRIORITY_HL_TIME_BASE configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 0
#define ISR_PRIORITY_PIO_ADC configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY - 1

void sleepAboutMS(int ms);

typedef enum { semTest = 0, semCount } t_semaphoreHandle;
typedef enum {
    taskHandleID_RPCSerialIn,
    taskHandleID_LED,
    taskHandleID_key,
    taskHandleID_adc,
    taskHandleID_external_adc,
    taskHandleID_display,
    taskHandleID_supervisor,
    taskHandleID_count
} t_taskHandleID;

void HALTAndBlink(void);

extern TaskHandle_t taskHandles[taskHandleID_count];

extern xSemaphoreHandle xSemaphores[semCount];
typedef enum {
    rer_none,
    rer_resetPin,
    rer_powerOnReset,
    rer_independendWatchdog,
    rer_windowWatchdog,
    rer_softwareReset,
    rer_rtc,
    rer_wupin1_USB,
    rer_wupin2_ONOFFKEY,
} resetReason_t;

extern resetReason_t mainResetReason;
extern channel_codec_instance_t cc_instances[channel_codec_comport_COUNT];

void main_set_date_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec);

extern bool DMA1_Stream2_IRQHandler_triggered;
#endif /* MAIN_H_ */
