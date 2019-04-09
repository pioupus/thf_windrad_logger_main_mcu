/*
 * task_supervisor.c
 *
 *  Created on: 02.06.2018
 *      Author: arne
 */

#include "board.h"
#include "task_supervisor.h"
#include "task_adc.h"
#include "rtc_stdlib.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// const uint16_t THRESHOLD_SUPPLY_SLEEP_10mV = 2400;

const uint16_t THRESHOLD_SUPPLY_VOLTAGE_OFF_10mV = 2400;
const uint16_t THRESHOLD_SUPPLY_VOLTAGE_ON_10mV = 2500;

const uint16_t TIME_FOR_RASPI_TO_SHUTDOWN_s = 30;
const uint8_t WAKEUPT_TIME_TO_PROBE_SUPPLY_VOLTAGE_s = 20;

static TickType_t time_of_shutdown_command_s = 0;

static void notify_raspy_to_shut_down(void) {
    time_of_shutdown_command_s = (xTaskGetTickCount() / portTICK_RATE_MS) / 1000;
    //   SET_RASP_IRQ();
}

static void notify_raspy_not_to_shut_down(void) {
    time_of_shutdown_command_s = 0;
    CLEAR_RASP_IRQ();
}

void superisor_check_supply_voltage() {
    static bool raspi_was_on_once = false;

    uint16_t values[adsi_max] = {0};
    adc_get_values(values);

    if (values[adsi_supply_sensse] < THRESHOLD_SUPPLY_VOLTAGE_OFF_10mV) {
        notify_raspy_to_shut_down();
        if (raspi_was_on_once == false) {
            time_of_shutdown_command_s += TIME_FOR_RASPI_TO_SHUTDOWN_s + 1;
        }
    }
    if (values[adsi_supply_sensse] > THRESHOLD_SUPPLY_VOLTAGE_ON_10mV) {
        raspi_was_on_once = true;
        CLEAR_SHUTDOWN();
        notify_raspy_not_to_shut_down();
    }
    if (time_of_shutdown_command_s) {
        if ((time_of_shutdown_command_s + TIME_FOR_RASPI_TO_SHUTDOWN_s) < ((xTaskGetTickCount() / portTICK_RATE_MS)) / 1000) {
#if 0
        	SET_SHUTDOWN();
              rtc_goto_standby_with_wakup_after_period(WAKEUPT_TIME_TO_PROBE_SUPPLY_VOLTAGE_s);
#endif
        }
    }
}

void taskSupervisor(void *pvParameters) {
#if 1
    CLEAR_RASP_IRQ();
    while (adc_values_valid() == false) {
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    while (1) {
        superisor_check_supply_voltage();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
#endif
}
