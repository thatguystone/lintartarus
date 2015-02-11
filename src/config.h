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
#include "usb.h"

/**
 * Default config file when one doesn't exist
 */
#define DEFAULT_CONFIG \
	"[default]\n" \
	"pulse = false\n" \
	"brightness = low\n" \
	"next-layout = ctrl+alt+shift+n\n" \
	"prev-layout = ctrl+alt+shift+p\n"

/**
 * Configuration options
 */
struct config{
	char *config_dir;
	int layout;

	struct {
		char *next;
		char *prev;
	} hotkeys;

	struct {
		int pulse;
		enum usb_brightness brightness;
	} usb;
};

/**
 * Global config
 */
struct config cfg;

/**
 * Setup global config
 */
void cfg_init(int argc, char **argv);

/**
 * Reload configuration files
 */
void cfg_reload(void);
