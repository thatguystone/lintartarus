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

#pragma once
#include <glib.h>

/**
 * Basic layout init
 */
void layout_init(void);

/**
 * Trigger the event corresponding to the given code
 */
const GPtrArray* layout_translate(int code);

/**
 * Handle an internal command for the layout
 */
void layout_handle_internal(int code);

/**
 * Handle config changes
 */
void layout_on_config_updated(void);

/**
 * A program started
 */
void layout_on_prog_start(void);

/**
 * A program ended
 */
void layout_on_prog_end(void);
