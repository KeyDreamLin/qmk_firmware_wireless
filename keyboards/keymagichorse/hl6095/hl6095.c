/* Copyright 2024 keymagichorse
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "quantum.h"
#include "battery.h"
#include "SEGGER_RTT.h"
void board_init(void) 
{
    SEGGER_RTT_Init();
    battery_init();
    SEGGER_RTT_printf(0,"hello rtt log????\r\n");
}


void housekeeping_task_kb(void) {
    battery_task();
}



void keyboard_post_init_kb(void)
{
}
