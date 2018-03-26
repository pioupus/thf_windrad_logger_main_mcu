/*
 * task_display.c
 *
 *  Created on: 18.02.2018
 *      Author: arne
 */

#include "hd44780.h"
#include "task_display.h"
#include "task_external_adc.h"
#include <assert.h>
#include "vc.h"

const uint8_t TABLE_3_COL_1 = 5;
const uint8_t TABLE_3_COL_2 = 10;
const uint8_t TABLE_3_COL_3 = 15;

screen_id_t current_screen_id = -1;

typedef struct {
    screen_id_t id;
    int update_interval_ms;
    void (*paint_function)(void);
    void (*update_function)(void);
} screen_description_t;

static void display_screen_iup_background() {
    lcd_putc('\f');
    lcd_printf_at(TABLE_3_COL_1, 0, "Uavg");
    lcd_printf_at(TABLE_3_COL_2, 0, "Iavg");
    lcd_printf_at(TABLE_3_COL_3, 0, "Ieff");
    lcd_printf_at(0, 1, "L1");
    lcd_printf_at(0, 2, "L2");
    lcd_printf_at(0, 3, "L3");
}

static void display_screen_iup_update() {
    static uint8_t i;

    int16_t u_avg[u_COUNT] = {0};
    uint16_t u_eff[3] = {0};
    int16_t i_avg[i_COUNT] = {0};
    uint16_t i_eff[i_COUNT] = {0};

    extadc_get_voltages_avg(u_avg);
    extadc_get_voltages_effective(u_eff);

    extadc_get_currents_avg(i_avg);
    extadc_get_currents_effective(i_eff);

    lcd_printf_at(TABLE_3_COL_1, 1, "    ");
    lcd_printf_at(TABLE_3_COL_1, 1, "%d", u_avg[u_l12]);

    lcd_printf_at(TABLE_3_COL_1, 2, "    ");
    lcd_printf_at(TABLE_3_COL_1, 2, "%d", u_avg[u_l23]);

    lcd_printf_at(TABLE_3_COL_1, 3, "    ");
    lcd_printf_at(TABLE_3_COL_1, 3, "%d", u_avg[u_l31]);

    lcd_printf_at(TABLE_3_COL_2, 1, "    ");
    lcd_printf_at(TABLE_3_COL_2, 1, "%d", i_avg[i_l1]);

    lcd_printf_at(TABLE_3_COL_2, 2, "    ");
    lcd_printf_at(TABLE_3_COL_2, 2, "%d", i_avg[i_l2]);

    lcd_printf_at(TABLE_3_COL_2, 3, "    ");
    lcd_printf_at(TABLE_3_COL_2, 3, "%d", i_avg[i_l3]);

    lcd_printf_at(TABLE_3_COL_3, 1, "    ");
    lcd_printf_at(TABLE_3_COL_3, 1, "%d", i_eff[i_l1]);

    lcd_printf_at(TABLE_3_COL_3, 2, "    ");
    lcd_printf_at(TABLE_3_COL_3, 2, "%d", i_eff[i_l2]);

    lcd_printf_at(TABLE_3_COL_3, 3, "    ");
    lcd_printf_at(TABLE_3_COL_3, 3, "%d", i_eff[i_l3]);
}

static void display_screen_boot_background() {
    lcd_putc('\f');
    lcd_printf_at(TABLE_3_COL_1, 0, "Enerlyzer");
    lcd_printf_at(0, 1, "git: %X", GITHASH);
    lcd_printf_at(0, 2, "gitdate: %s", GITDATE);
    lcd_printf_at(0, 3, "kruegerarneak@gmail");
}

static void display_screen_boot_update() {
}

const screen_description_t screen_instruction_table[] = {
    //
    {.id = screen_boot,
     .update_interval_ms = 5000,
     .paint_function = &display_screen_boot_background,
     .update_function = &display_screen_boot_update},
    {.id = screen_iup, .update_interval_ms = 1000, .paint_function = &display_screen_iup_background, .update_function = &display_screen_iup_update} //
};

void display_set_screen(screen_id_t new_screen_id) {
    if (current_screen_id != new_screen_id) {
        current_screen_id = new_screen_id;
        screen_instruction_table[current_screen_id].paint_function();
        screen_instruction_table[current_screen_id].update_function();
    }
}

static void test_screen_isntruction_table() {
    for (int i = 0; i < screen_COUNT - 1; i++) {
        assert(screen_instruction_table[i].id == i);
    }
}

void taskDisplay(void *pvParameters) {
#if 1

    lcd_init();
    test_screen_isntruction_table();
    display_set_screen(screen_boot);
    vTaskDelay((screen_instruction_table[screen_boot].update_interval_ms / portTICK_RATE_MS));

    display_set_screen(screen_iup);
    while (1) {
        screen_instruction_table[current_screen_id].update_function();
        vTaskDelay((screen_instruction_table[current_screen_id].update_interval_ms / portTICK_RATE_MS));
    }
#endif
}
