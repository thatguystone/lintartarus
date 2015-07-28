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

/**
 * Key code for changing to next layout
 */
#define KEY_NEXT_LAYOUT -2

/**
 * Key code for changing to previous layout
 */
#define KEY_PREV_LAYOUT -1

/**
 * Get the corresponding keycode for the given key name, or -1 if it doesn't
 * exist.
 */
int keys_code(const char *name);

/**
 * The opposite of keys_code().
 */
const char* keys_val(const int code);

/**
 * Get the name of the key, as printed on the device.
 */
const char* keys_get_dev_name(const guint i);

/**
 * Get the default key name for the pad at the given index.
 */
const char* keys_get_dev_default(const guint i);

/**
 * Parse a human-readable sequence of keys into a machine-usable sequence.
 */
gboolean keys_parse(const char *keys, GPtrArray **combo);

/**
 * Dump a human-reasable key sequence
 */
char* keys_dump(GPtrArray *combo);

/**
 * Get all known key codes.
 *
 * Free the array when you're done.
 */
GArray* keys_get_all_codes(void);
