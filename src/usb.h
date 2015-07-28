/*
 * lintartarus: key mapping and light control for the Razer Tartarus on Linux
 * Copyright (C) 2015 Andrew Stone <a@stoney.io>
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

#pragma once
#include <glib.h>

/**
 * Different backlight levels
 */
enum usb_backlight {
	backlight_off,
	backlight_low,
	backlight_med,
	backlight_high,
	backlight_pulse,
};

/**
 * Get USB ready to run
 */
void usb_init(void);

/**
 * State changed. Update device.
 */
void usb_on_state_changed(void);

/**
 * Just check USB state
 */
void usb_on_poll_tick(void);

/**
 * Print a USB error to stderr
 */
G_GNUC_PRINTF(2, 3)
void usb_perror(int err, const char *format, ...);
