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
#include "layout.h"
#include "usb.h"

/**
 * Default config file when one doesn't exist
 */
#define DEFAULT_CONFIG \
	"[default]\n" \
	"pulse = false\n" \
	"brightness = low\n" \
	"launch = ctrl+alt+shift+l\n" \
	"next-layout = ctrl+alt+shift+n\n" \
	"prev-layout = ctrl+alt+shift+p\n"

/**
 * A configured program, complete with layouts and everything!
 */
struct program {
	/**
	 * User-configured name
	 */
	char *name;

	/**
	 * A bunch of strings
	 */
	GPtrArray *cmds;

	/**
	 * Even more strings
	 */
	GPtrArray *exes;

	/**
	 * A bunch of layouts
	 */
	GPtrArray *layouts;
};

/**
 * Configuration options
 */
struct config{
	char *config_dir;

	int layout;

	GPtrArray *programs;

	struct {
		char *launch;
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
 * Dump out parsed config values
 */
void cfg_dump(void);

/**
 * Get fd to poll on
 */
int cfg_fd(void);

/**
 * Reload configuration files
 */
void cfg_reload(void);
