/*
 * lintartarus: key mapping and light control for the Razer Tartarus on Linux
 * Copyright (C) 2015 Andrew Stone <andrew@clovar.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <stdio.h>
#include "layout.h"

void layout_next()
{
	printf("layout_next\n");
}

void layout_prev()
{
	printf("layout_prev\n");
}

const char* layout_get_name(const guint i)
{
	switch (i) {
		// Keypad
		case 0:  return "1";
		case 1:  return "2";
		case 2:  return "3";
		case 3:  return "4";
		case 4:  return "5";
		case 5:  return "6";
		case 6:  return "7";
		case 7:  return "8";
		case 8:  return "9";
		case 9:  return "10";
		case 10: return "11";
		case 11: return "12";
		case 12: return "13";
		case 13: return "14";
		case 14: return "15";

		// Arrows
		case 15: return "up";
		case 16: return "down";
		case 17: return "left";
		case 18: return "right";

		// Thumb buttons
		case 19: return "thumb_up";
		case 20: return "thumb_down";

		// Or you fucked up
		default: g_error("key %u doesn't exist. moron.", i);
	}
}

char* layout_get_default(const guint i)
{
	switch (i) {
		// Keypad
		case 0:  return g_strdup("tab");
		case 1:  return g_strdup("q");
		case 2:  return g_strdup("w");
		case 3:  return g_strdup("e");
		case 4:  return g_strdup("r");
		case 5:  return g_strdup("capslock");
		case 6:  return g_strdup("a");
		case 7:  return g_strdup("s");
		case 8:  return g_strdup("d");
		case 9: return g_strdup("f");
		case 10: return g_strdup("shift");
		case 11: return g_strdup("z");
		case 12: return g_strdup("x");
		case 13: return g_strdup("c");
		case 14: return g_strdup("v");

		// Arrows
		case 15: return g_strdup("up");
		case 16: return g_strdup("down");
		case 17: return g_strdup("left");
		case 18: return g_strdup("right");

		// Thumb buttons
		case 19: return g_strdup("alt");
		case 20: return g_strdup("space");

		// Or you fucked up
		default: g_error("key %u doesn't exist. moron.", i);
	}
}
