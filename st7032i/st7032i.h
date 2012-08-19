/**
 * @file	st7032i.h
 * @brief	ST7032iアクセス
 * 
 * http://strawberry-linux.com/catalog/items?code=27001
 */
#ifndef ST7032I_H
#define ST7032I_H

#include <kernel.h>

void st7032i_hw_init(intptr_t exinf);

void st7032i_task_init(void);
void st7032i_clear(void);
void st7032i_move_pos(int x, int y);
void st7032i_write_string(const char* pStr);

#endif /* ST7032I_H */
