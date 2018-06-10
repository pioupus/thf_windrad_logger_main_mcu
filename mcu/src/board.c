/*
 *      Author: ak
 * Copyright (C) 2015  Crystal-Photonics GmbH
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

const pinGPIO_t gpioPins[] = {
#if 1
    PIN_DISP_D4, PIN_DISP_D5, PIN_DISP_D6, PIN_DISP_D7, PIN_DISP_RS, PIN_DISP_E,
    //
    PIN_GPIO0, PIN_GPIO1, PIN_GPIO2, PIN_GPIO3, PIN_GPIO4, PIN_WS2812,                                                                        //
    PIN_LED_RED, PIN_LED_GREEN, PIN_LED_YELLOW, PIN_LED_COM_TOP_YELLOW, PIN_LED_COM_TOP_GREEN, PIN_LED_COM_BOT_YELLOW, PIN_LED_COM_BOT_GREEN, //
    PIN_SHUTDOWN, PIN_SUPPLY_SENSE, PIN_ZERO_CROSS,                                                                                           //
    PIN_SD_DETECT, PIN_CS_SD, PIN_CS_ADC, PIN_KEYONOFF, PIN_KEY_1, PIN_KEY_2, PIN_KEY_3, PIN_DIP_0, PIN_DIP_1, PIN_DIP_2, PIN_DIP_3,          //
    PIN_WIND_DIR_GRAY_0, PIN_WIND_DIR_GRAY_1, PIN_WIND_DIR_GRAY_2, PIN_WIND_DIR_GRAY_3, PIN_WIND_DIR_GRAY_4, PIN_WIND_DIR_GRAY_5, PIN_WIND_DIR_GRAY_6,
    PIN_WIND_DIR_GRAY_7,                                //
    PIN_WIND_SPEED_PULS, PIN_WIND_HEATER, PIN_RASP_IRQ, //

    PIN_RMII_REF_CLK, PIN_RMII_MDIO, PIN_RMII_CRS, PIN_RMII_MDC, PIN_RMII_TX_EN, PIN_RMII_TX0, PIN_RMII_TX1, PIN_RMII_RX0, PIN_RMII_RX1,

    PIN_MCU_TX_ENABLE, PIN_USB_POWER_ON, PIN_USB_POWER_FAIL,

    PIN_ADC_CURR_EXT
#else
    PIN_DISCO_LED_RED,

    PIN_DISCO_LED_GREEN

#endif

    // PIN_ADC_SCK_PIO, PIN_ADC_MISO_PIO, PIN_ADC_MOSI_PIO, PIN_CS_ADC_PIO //
};

const pinGPIO_t afPins[] = {

    PIN_CS_ADC, PIN_ADC_SCK, PIN_ADC_MISO, PIN_ADC_MOSI,

    PIN_MCU_RX, PIN_MCU_TX,

    PIN_DBG_RX, PIN_DBG_TX
#if 0
	,	PIN_RASP_CS, PIN_RASP_SCK, PIN_RASP_MOSI, PIN_RASP_MISO,

		PIN_USB_N,PIN_USB_P,PIN_USB_ID,PIN_USB_VBUS,



		PIN_MCU_SDA,PIN_MCU_SCL

#else

#endif
};

void boardPowerOn(void) {

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();

    // __GPIOA_CLK_SLEEP_ENABLE();
    //__GPIOB_CLK_SLEEP_ENABLE();

    __TIM9_CLK_ENABLE();
}

void boardConfigurePIO(void) {
    size_t i;

    boardPowerOn();

    for (i = 0; i < sizeof(gpioPins) / sizeof(pinGPIO_t); i++) {
        boardInitPin(&gpioPins[i]);
    }

    for (i = 0; i < sizeof(afPins) / sizeof(pinGPIO_t); i++) {
        boardInitAFPin(&afPins[i]);
    }
}

void SET_SHUTDOWN() {
    static pinGPIO_t shutdown_pin_as_input = PIN_SHUTDOWN_IN;
    boardInitPin(&shutdown_pin_as_input);
}

void CLEAR_SHUTDOWN() {
    static pinGPIO_t shutdown_pin_as_output = PIN_SHUTDOWN;
    boardInitPin(&shutdown_pin_as_output);
    CLEAR_SHUTDOWN_();
}

void boardInitPin(const pinGPIO_t *pin) {
    HAL_GPIO_Init(pin->port, (GPIO_InitTypeDef *)&pin->pinDef);
}

void boardInitAFPin(const pinGPIO_t *pinAF) {
    HAL_GPIO_Init(pinAF->port, (GPIO_InitTypeDef *)&pinAF->pinDef);
}

void boardDisplayWriteData(uint8_t data) {
    data &= 0x0F;
    uint32_t sets = data;
    uint32_t clears = ~data;

    sets &= 0x0F;
    clears &= 0x0F;
    uint32_t clears_n_sets = (clears << 16) | sets;

    GPIOD->BSRR = clears_n_sets;

    // GPIOD->ODR |= data;
}

#if 1

bool boardTestKey(key_id_t keyID) {
    switch (keyID) {

        case kid_onoff:
            if (GET_KEYONOFF())
                return true;
            else
                return false;
            break;

        case kid_key1:
            if (GET_KEY_1())
                return true;
            else
                return false;
            break;
        case kid_key2:
            if (GET_KEY_2())
                return true;
            else
                return false;
            break;
        case kid_key3:
            if (GET_KEY_3())
                return true;
            else
                return false;
            break;
        case kid_KEYCOUNT:
        case kid_none:
            return false;
    }
    return false;
}
#endif

void boardSetLED(led_id_t ledID, led_state_t ledState) {
    switch (ledID) {
        case lid_red:
#if 1
            if (ledState == les_on) {
                SET_LED_RED();
                // SET_DISCO_LED_RED();
            } else {
                CLEAR_LED_RED();
                // CLEAR_DISCO_LED_RED();
            }
#endif
            break;

        case lid_green:
#if 1
            if (ledState == les_on) {
                SET_LED_GREEN();
                // SET_DISCO_LED_GREEN();
            } else {
                CLEAR_LED_GREEN();
                // CLEAR_DISCO_LED_GREEN();
            }
#endif
            break;

        case lid_yellow:
#if 1
            if (ledState == les_on)
                SET_LED_YELLOW();
            else
                CLEAR_LED_YELLOW();
#endif
            break;
        case lid_LEDCOUNT:
        case lid_none:
            break;
    }
}
