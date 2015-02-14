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

#include <glib.h>
#include <linux/input.h>
#include "keys.h"

struct _pair {
	const char *name;
	const char *alt_name;
	int code;
};

/**
 * All supported keys
 */
struct _pair _pairs[] = {
	/*
	 * Internal key codes
	 */

	{	.name = "LAYOUT_NEXT",
		.code = KEY_NEXT_LAYOUT,
	},
	{	.name = "LAYOUT_PREV",
		.code = KEY_PREV_LAYOUT,
	},

	/*
	 * OS key codes
	 */

	{	.name = "ESC",
		.code = 1,
	},
	{	.name = "1",
		.code = 2,
	},
	{	.name = "2",
		.code = 3,
	},
	{	.name = "3",
		.code = 4,
	},
	{	.name = "4",
		.code = 5,
	},
	{	.name = "5",
		.code = 6,
	},
	{	.name = "6",
		.code = 7,
	},
	{	.name = "7",
		.code = 8,
	},
	{	.name = "8",
		.code = 9,
	},
	{	.name = "9",
		.code = 10,
	},
	{	.name = "0",
		.code = 11,
	},
	{	.name = "-",
		.code = 12,
	},
	{	.name = "=",
		.code = 13,
	},
	{	.name = "BACKSPACE",
		.code = 14,
	},
	{	.name = "TAB",
		.code = 15,
	},
	{	.name = "Q",
		.code = 16,
	},
	{	.name = "W",
		.code = 17,
	},
	{	.name = "E",
		.code = 18,
	},
	{	.name = "R",
		.code = 19,
	},
	{	.name = "T",
		.code = 20,
	},
	{	.name = "Y",
		.code = 21,
	},
	{	.name = "U",
		.code = 22,
	},
	{	.name = "I",
		.code = 23,
	},
	{	.name = "O",
		.code = 24,
	},
	{	.name = "P",
		.code = 25,
	},
	{	.name = "[",
		.code = 26,
	},
	{	.name = "]",
		.code = 27,
	},
	{	.name = "ENTER",
		.code = 28,
	},
	{	.name = "CTRL_L",
		.alt_name = "CTRL",
		.code = 29,
	},
	{	.name = "A",
		.code = 30,
	},
	{	.name = "S",
		.code = 31,
	},
	{	.name = "D",
		.code = 32,
	},
	{	.name = "F",
		.code = 33,
	},
	{	.name = "G",
		.code = 34,
	},
	{	.name = "H",
		.code = 35,
	},
	{	.name = "J",
		.code = 36,
	},
	{	.name = "K",
		.code = 37,
	},
	{	.name = "L",
		.code = 38,
	},
	{	.name = ";",
		.code = 39,
	},
	{	.name = "\'",
		.code = 40,
	},
	{	.name = "`",
		.code = 41,
	},
	{	.name = "SHIFT_L",
		.alt_name = "SHIFT",
		.code = 42,
	},
	{	.name = "\\",
		.code = 43,
	},
	{	.name = "Z",
		.code = 44,
	},
	{	.name = "X",
		.code = 45,
	},
	{	.name = "C",
		.code = 46,
	},
	{	.name = "V",
		.code = 47,
	},
	{	.name = "B",
		.code = 48,
	},
	{	.name = "N",
		.code = 49,
	},
	{	.name = "M",
		.code = 50,
	},
	{	.name = ",",
		.code = 51,
	},
	{	.name = ".",
		.code = 52,
	},
	{	.name = "/",
		.code = 53,
	},
	{	.name = "SHIFT_R",
		.code = 54,
	},
	{	.name = "*",
		.code = 55,
	},
	{	.name = "ALT_L",
		.alt_name = "ALT",
		.code = 56,
	},
	{	.name = "SPACE",
		.code = 57,
	},
	{	.name = "CAPSLOCK",
		.code = 58,
	},
	{	.name = "F1",
		.code = 59,
	},
	{	.name = "F2",
		.code = 60,
	},
	{	.name = "F3",
		.code = 61,
	},
	{	.name = "F4",
		.code = 62,
	},
	{	.name = "F5",
		.code = 63,
	},
	{	.name = "F6",
		.code = 64,
	},
	{	.name = "F7",
		.code = 65,
	},
	{	.name = "F8",
		.code = 66,
	},
	{	.name = "F9",
		.code = 67,
	},
	{	.name = "F10",
		.code = 68,
	},
	{	.name = "NUMLOCK",
		.code = 69,
	},
	{	.name = "SCROLLLOCK",
		.code = 70,
	},
	{	.name = "KP7",
		.code = 71,
	},
	{	.name = "KP8",
		.code = 72,
	},
	{	.name = "KP9",
		.code = 73,
	},
	{	.name = "KP-",
		.code = 74,
	},
	{	.name = "KP4",
		.code = 75,
	},
	{	.name = "KP5",
		.code = 76,
	},
	{	.name = "KP6",
		.code = 77,
	},
	{	.name = "PLUS",
		.code = 78,
	},
	{	.name = "KP1",
		.code = 79,
	},
	{	.name = "KP2",
		.code = 80,
	},
	{	.name = "KP3",
		.code = 81,
	},
	{	.name = "KP0",
		.code = 82,
	},
	{	.name = "KPD.",
		.code = 83,
	},
	{	.name = "F11",
		.code = 87,
	},
	{	.name = "F12",
		.code = 88,
	},
	{	.name = "RO",
		.code = 89,
	},
	{	.name = "KPENTER",
		.code = 96,
	},
	{	.name = "CTRL_R",
		.code = 97,
	},
	{	.name = "KP/",
		.code = 98,
	},
	{	.name = "ALT_R",
		.code = 100,
	},
	{	.name = "HOME",
		.code = 102,
	},
	{	.name = "UP",
		.code = 103,
	},
	{	.name = "PAGEUP",
		.code = 104,
	},
	{	.name = "LEFT",
		.code = 105,
	},
	{	.name = "RIGHT",
		.code = 106,
	},
	{	.name = "END",
		.code = 107,
	},
	{	.name = "DOWN",
		.code = 108,
	},
	{	.name = "PAGEDOWN",
		.code = 109,
	},
	{	.name = "INSERT",
		.code = 110,
	},
	{	.name = "DELETE",
		.code = 111,
	},
	{	.name = "KP=",
		.code = 117,
	},
	{	.name = "KPPLUSMINUS",
		.code = 118,
	},
	{	.name = "PAUSE",
		.code = 119,
	},
	{	.name = "KP,",
		.code = 121,
	},
	{	.name = "SUPER-L",
		.alt_name = "SUPER",
		.code = 125,
	},
	{	.name = "SUPER-R",
		.code = 126,
	},
	{	.name = "COMPOSE",
		.code = 127,
	},
	{	.name = "F13",
		.code = 183,
	},
	{	.name = "F14",
		.code = 184,
	},
	{	.name = "F15",
		.code = 185,
	},
	{	.name = "F16",
		.code = 186,
	},
	{	.name = "F17",
		.code = 187,
	},
	{	.name = "F18",
		.code = 188,
	},
	{	.name = "F19",
		.code = 189,
	},
	{	.name = "F20",
		.code = 190,
	},
	{	.name = "F21",
		.code = 191,
	},
	{	.name = "F22",
		.code = 192,
	},
	{	.name = "F23",
		.code = 193,
	},
	{	.name = "F24",
		.code = 194,
	},
};

static void _combo_free(void *seq_)
{
	GArray *seq = seq_;
	g_array_free(seq, TRUE);
}

int keys_code(const char *name)
{
	uint i;
	int code = 0;
	char *key = g_ascii_strup(name, -1);

	for (i = 0; i < G_N_ELEMENTS(_pairs); i++) {
		if (g_strcmp0(_pairs[i].name, key) == 0 ||
			g_strcmp0(_pairs[i].alt_name, key) == 0) {

			code = _pairs[i].code;
			break;
		}
	}

	g_free(key);

	return code;
}

const char* keys_val(const int code)
{
	uint i;
	const char *name = "UNKNOWN";

	for (i = 0; i < G_N_ELEMENTS(_pairs); i++) {
		if (_pairs[i].code == code) {
			if (_pairs[i].alt_name != NULL) {
				name = _pairs[i].alt_name;
			} else {
				name = _pairs[i].name;
			}
			break;
		}
	}

	return name;
}

const char* keys_get_dev_name(const guint i)
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

const char* keys_get_dev_default(const guint i)
{
	switch (i) {
		// Keypad
		case 0:  return "tab";
		case 1:  return "q";
		case 2:  return "w";
		case 3:  return "e";
		case 4:  return "r";
		case 5:  return "capslock";
		case 6:  return "a";
		case 7:  return "s";
		case 8:  return "d";
		case 9:  return "f";
		case 10: return "shift";
		case 11: return "z";
		case 12: return "x";
		case 13: return "c";
		case 14: return "v";

		// Arrows
		case 15: return "up";
		case 16: return "down";
		case 17: return "left";
		case 18: return "right";

		// Thumb buttons
		case 19: return "alt";
		case 20: return "space";

		// Or you fucked up
		default: g_error("key %u doesn't exist. moron.", i);
	}
}

gboolean keys_parse(const char *keys, GPtrArray **combo)
{
	uint i;
	uint j;
	uint len;
	uint klen;
	gboolean ok = FALSE;
	char **combos = g_strsplit(keys, " \t", 0);

	*combo = g_ptr_array_new_with_free_func(_combo_free);

	len = g_strv_length(combos);
	for (i = 0; i < len; i++) {
		GArray *seq = g_array_new(TRUE, TRUE, sizeof(int));
		char **ks = g_strsplit(combos[i], "+", 0);

		g_ptr_array_add(*combo, seq);

		klen = g_strv_length(ks);
		for (j = 0; j < klen; j++) {
			int code = keys_code(ks[j]);
			if (code == 0) {
				g_critical("invalid key in combo %s: %s", keys, ks[j]);
				goto out;
			}

			g_array_append_val(seq, code);
		}

		g_strfreev(ks);
	}

	ok = TRUE;

out:
	if (!ok) {
		g_ptr_array_free(*combo, TRUE);
		*combo = NULL;
	}

	g_strfreev(combos);

	return ok;
}

char* keys_dump(GPtrArray *combo)
{
	uint i;
	uint j;
	char *d;
	GString *buff = g_string_new("");

	for (i = 0; i < combo->len; i++) {
		GArray *seq = g_ptr_array_index(combo, i);

		for (j = 0; j < seq->len; j++) {
			int code = g_array_index(seq, int, j);

			g_string_append_printf(buff, "+%s",
				keys_val(code));
		}

		g_string_append_c(buff, ' ');
	}

	g_string_erase(buff, 0, 1);
	g_strstrip(buff->str);

	d = g_ascii_strdown(buff->str, buff->len);
	g_string_free(buff, TRUE);

	return d;
}

GArray* keys_get_all_codes()
{
	guint i;
	GArray *a = g_array_new(FALSE, FALSE, sizeof(int));

	for (i = 0; i < G_N_ELEMENTS(_pairs); i++) {
		g_array_append_val(a, _pairs[i].code);
	}

	return a;
}
