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

#include <errno.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "uinput.h"

struct _pair {
	const char *name;
	const char *alt_name;
	int code;
};

/**
 * All supported keys
 */
struct _pair _pairs[] = {
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
	{	.name = "KP+",
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
	{	.name = "KP+-",
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

static const char *_uinput_paths[] = {
	"/dev/uinput",
	"/dev/input/uinput",
	"/dev/misc/uinput",
};

/**
 * Created output keyboard
 */
static int _out;

void uinput_init(void)
{
	uint i;
	int fd;
	int err;
	struct uinput_user_dev uidev = {
		.name = "lintartarus keyboard",
		.id = {
			.bustype = BUS_VIRTUAL,
			.vendor = 0x1337,
			.product = 0xb33f,
			.version = 1,
		},
	};

	fd = -1;
	for (i = 0; fd == -1 && i < G_N_ELEMENTS(_uinput_paths); i++) {
		fd = open(_uinput_paths[i], O_WRONLY | O_NONBLOCK);
		if (fd == -1) {
			if (errno == EACCES) {
				g_error("failed to open uinput device: %s", strerror(errno));
			}
		}
	}

	if (fd == -1) {
		g_error("uinput device not found. did you `modprobe uinput`?");
	}

	err = write(fd, &uidev, sizeof(uidev));
	if (err == -1) {
		g_error("failed to init uinput keyboard: %s", strerror(errno));
	}

	err = ioctl(fd, UI_SET_EVBIT, EV_KEY);
	if (err == -1) {
		g_error("failed to setup uinput keyboard: %s", strerror(errno));
	}

	for (i = 0; i < G_N_ELEMENTS(_pairs); i++) {
		err = ioctl(fd, UI_SET_KEYBIT, _pairs[i].code);
		if (err == -1) {
			g_error("failed to setup key %s: %s",
				_pairs[i].name,
				strerror(errno));
		}
	}

	err = ioctl(fd, UI_DEV_CREATE);
	if (err == -1) {
		g_error("failed to create uinput keyboard: %s", strerror(errno));
	}

	_out = fd;

	while (TRUE) {
		struct input_event ev = {
			.type = EV_KEY,
			.code = _pairs[88].code,
			.value = 1,
		};

		write(_out, &ev, sizeof(ev));
		usleep(100);

		ev.value = 0;
		write(_out, &ev, sizeof(ev));

		usleep(1000 * 100);
	}

	// fd = open("/dev/input/event21", O_RDWR);
	// ioctl(fd, EVIOCGRAB, 1);

	// fd = open("/dev/input/event22", O_RDWR);
	// ioctl(fd, EVIOCGRAB, 1);

	// fd = open("/dev/input/event23", O_RDWR);
	// ioctl(fd, EVIOCGRAB, 1);

	// while (1);
}
