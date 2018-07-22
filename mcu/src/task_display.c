/*
 * task_display.c
 *
 *  Created on: 18.02.2018
 *      Author: arne
 */

#include "hd44780.h"
#include "task_display.h"
#include "task_external_adc.h"
#include "task_adc.h"
#include <assert.h>
#include "vc.h"
#include <time.h>
#include <string.h>

const uint8_t TABLE_3_COL_1 = 5;
const uint8_t TABLE_3_COL_2 = 10;
const uint8_t TABLE_3_COL_3 = 15;

screen_id_t current_screen_id = -1;

#define COUNT_OF_POSSIBLE_SYSSTAT_SCREENS 3

static uint8_t sysstat_screen[COUNT_OF_POSSIBLE_SYSSTAT_SCREENS][_LCD_ROWS][_LCD_COLS];
static uint8_t used_sysstat_screens = 0;

static const uint16_t SWITCH_SCREEN_INTERVAL = 10;

static uint8_t custom_screen_buffer[_LCD_ROWS][_LCD_COLS];

typedef enum { update_reason_just_set, update_reason_regular } screen_update_reason_t;
typedef struct {
    screen_id_t id;
    int update_interval_ms;
    void (*paint_function)(void);
    void (*update_function)(screen_update_reason_t);
} screen_description_t;

void display_custom_screen_write_text(uint8_t row, uint8_t *text) {
    memcpy(custom_screen_buffer[row], text, _LCD_COLS);
}

void display_custom_screen_clear() {
    memset(custom_screen_buffer, 0, sizeof(custom_screen_buffer));
}

void display_set_sysstat_screen_(uint8_t count_of_screens, uint8_t screen_index, uint8_t row, uint8_t text[20]) {
    assert(count_of_screens <= COUNT_OF_POSSIBLE_SYSSTAT_SCREENS);
    assert(screen_index < COUNT_OF_POSSIBLE_SYSSTAT_SCREENS);

    used_sysstat_screens = count_of_screens;

    memcpy(sysstat_screen[screen_index][row], text, _LCD_COLS);
}

static void display_screen_custom_background() {
}

static void display_screen_custom_update(screen_update_reason_t update_reason) {
    (void)update_reason;
    lcd_putc('\f');
    for (int i = 0; i < 4; i++) {
        lcd_printf_at(0, i, "%s", custom_screen_buffer[i][0]);
    }
}

static void display_screen_sys_stat_background() {
}

static void display_screen_sys_stat_update(screen_update_reason_t update_reason) {
    static uint8_t current_sysstat_screens_index = 0;
    if (update_reason == update_reason_regular) {
        current_sysstat_screens_index++;
    }
    if (current_sysstat_screens_index >= used_sysstat_screens) {
        display_set_screen(screen_iu);
        current_sysstat_screens_index = 0;
        return;
    }
    lcd_putc('\f');
    for (int i = 0; i < 4; i++) {
        lcd_printf_at(0, i, "%s", sysstat_screen[current_sysstat_screens_index][i]);
    }
}

static void display_screen_iu_background() {
    lcd_putc('\f');
    lcd_printf_at(TABLE_3_COL_1, 0, "Uavg");
    lcd_printf_at(TABLE_3_COL_2, 0, "Iavg");
    lcd_printf_at(TABLE_3_COL_3, 0, "Ieff");
    lcd_printf_at(0, 1, "L1");
    lcd_printf_at(0, 2, "L2");
    lcd_printf_at(0, 3, "L3");
}

static void display_screen_iu_update(screen_update_reason_t update_reason) {
    static int update_count = 0;
    if (update_reason == update_reason_regular) {
        update_count++;
    }
#if 1
    if (update_count > SWITCH_SCREEN_INTERVAL) {
        display_set_screen(screen_pbt);
        update_count = 0;
        return;
    }
#endif
    int16_t u_avg[u_COUNT] = {0};
    uint16_t u_eff[3] = {0};
    int16_t i_avg[i_COUNT] = {0};
    uint16_t i_eff[i_COUNT] = {0};

    extadc_get_voltages_avg(u_avg);
    extadc_get_voltages_effective(u_eff);

    extadc_get_currents_avg(i_avg);
    extadc_get_currents_effective(i_eff);

    lcd_printf_at(TABLE_3_COL_1, 1, "     ");
    lcd_printf_at(TABLE_3_COL_1, 1, "%d", u_avg[u_l21]);

    lcd_printf_at(TABLE_3_COL_1, 2, "     ");
    lcd_printf_at(TABLE_3_COL_1, 2, "%d", u_avg[u_l32]);

    lcd_printf_at(TABLE_3_COL_1, 3, "     ");
    lcd_printf_at(TABLE_3_COL_1, 3, "%d", u_avg[u_l13]);

    lcd_printf_at(TABLE_3_COL_2, 1, "     ");
    lcd_printf_at(TABLE_3_COL_2, 1, "%d", i_avg[i_l1]);

    lcd_printf_at(TABLE_3_COL_2, 2, "     ");
    lcd_printf_at(TABLE_3_COL_2, 2, "%d", i_avg[i_l2]);

    lcd_printf_at(TABLE_3_COL_2, 3, "     ");
    lcd_printf_at(TABLE_3_COL_2, 3, "%d", i_avg[i_l3]);

    lcd_printf_at(TABLE_3_COL_3, 1, "     ");
    lcd_printf_at(TABLE_3_COL_3, 1, "%d", i_eff[i_l1]);

    lcd_printf_at(TABLE_3_COL_3, 2, "     ");
    lcd_printf_at(TABLE_3_COL_3, 2, "%d", i_eff[i_l2]);

    lcd_printf_at(TABLE_3_COL_3, 3, "     ");
    lcd_printf_at(TABLE_3_COL_3, 3, "%d", i_eff[i_l3]);
}

static void display_screen_pbt_background() {
    lcd_putc('\f');
    lcd_printf_at(0, 0, "Time");
    lcd_printf_at(0, 1, "Pwr");
    lcd_printf_at(0, 2, "USup");
    lcd_printf_at(10, 2, "UBuf");
    lcd_printf_at(0, 3, "Temp");
    lcd_printf_at(10, 3, "DCI");
}

static void display_screen_pbt_update(screen_update_reason_t update_reason) {
    static int update_count = 0;
    if (update_reason == update_reason_regular) {
        update_count++;
    }
    if (update_count > SWITCH_SCREEN_INTERVAL) {
        display_set_screen(screen_sysstat);
        update_count = 0;
        return;
    }
    int32_t power = extadc_get_power();

    uint16_t values[adsi_max] = {0};
    adc_get_values(values);

    int32_t u_supply = values[adsi_supply_sensse];
    int32_t temperature = values[adsi_temperature];
    int32_t ext_current = values[adsi_curr_ext];
    int32_t u_coin = values[adsi_coin_cell];

    lcd_printf_at(TABLE_3_COL_1, 1, "     ");
    lcd_printf_at(TABLE_3_COL_1, 1, "%d", power);

    lcd_printf_at(TABLE_3_COL_1, 2, "     ");
    lcd_printf_at(TABLE_3_COL_1, 2, "%d", u_supply);

    lcd_printf_at(10 + 5, 2, "     ");
    lcd_printf_at(10 + 5, 2, "%d", u_coin);

    lcd_printf_at(TABLE_3_COL_1, 3, "     ");
    lcd_printf_at(TABLE_3_COL_1, 3, "%d", temperature);

    lcd_printf_at(10 + 5, 3, "     ");
    lcd_printf_at(10 + 5, 3, "%d", ext_current);

    time_t t = time(0);
    struct tm printTm = *(localtime(&t));
    char buffer[20] = {0};
    // strftime(buffer, 20, "%m/%d/%y %H:%M:%S", &printTm);
    strftime(buffer, 20, "%y/%m/%d/ %H:%M:%S", &printTm);
    lcd_printf_at(0, 0, "%s\r\n", buffer);
}

static void display_screen_boot_background() {
    lcd_putc('\f');
    lcd_printf_at(TABLE_3_COL_1, 0, "Enerlyzer");
    lcd_printf_at(0, 1, "git: %X", GITHASH);
    lcd_printf_at(0, 2, "gitdate: %s", GITDATE);
    lcd_printf_at(0, 3, "kruegerarneak@gmail");
}

static void display_screen_boot_update(screen_update_reason_t update_reason) {
}

const screen_description_t screen_instruction_table[] = {
    //
    {.id = screen_boot, //
     .update_interval_ms = 5000,
     .paint_function = &display_screen_boot_background,
     .update_function = &display_screen_boot_update},
    {.id = screen_iu, //
     .update_interval_ms = 1000,
     .paint_function = &display_screen_iu_background,
     .update_function = &display_screen_iu_update}, //
    {.id = screen_pbt,                              //
     .update_interval_ms = 1000,
     .paint_function = &display_screen_pbt_background,
     .update_function = &display_screen_pbt_update}, //

    {.id = screen_sysstat, //
     .update_interval_ms = 5000,
     .paint_function = &display_screen_sys_stat_background,
     .update_function = &display_screen_sys_stat_update}, //

    {.id = screen_custom, //
     .update_interval_ms = 1000,
     .paint_function = &display_screen_custom_background,
     .update_function = &display_screen_custom_update} //

};

void display_set_screen(screen_id_t new_screen_id) {
    if (current_screen_id != new_screen_id) {
        current_screen_id = new_screen_id;
        screen_instruction_table[current_screen_id].paint_function();
        screen_instruction_table[current_screen_id].update_function(update_reason_just_set);
    }
}

screen_id_t display_get_current_screen(void) {
    return current_screen_id;
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

    display_set_screen(screen_iu);
    while (1) {

        screen_instruction_table[current_screen_id].update_function(update_reason_regular);
        vTaskDelay((screen_instruction_table[current_screen_id].update_interval_ms / portTICK_RATE_MS));
    }
#endif
}
