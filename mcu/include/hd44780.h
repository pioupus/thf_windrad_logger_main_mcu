/*
 * hd44780.h
 *
 *  Created on: 28.01.2018
 *      Author: arne
 */

#ifndef MCU_INCLUDE_HD44780_H_
#define MCU_INCLUDE_HD44780_H_

#include <stdint.h>
void lcd_init(void);

void lcd_putc(char chr);

void lcd_locate(uint8_t col, /* Cursor column position (0.._LCD_COLS-1) */
                uint8_t row  /* Cursor row position (0.._LCD_ROWS-1) */
                );

void lcd_printf(const char *format, ...);
void lcd_printf_at(int col, int row, const char *format, ...);
#endif /* MCU_INCLUDE_HD44780_H_ */
