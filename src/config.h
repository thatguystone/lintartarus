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
#include "layout.h"
#include "usb.h"

/**
 * Default config file when one doesn't exist
 */
#define DEFAULT_CONFIG \
	"[default]\n" \
	"backlight = low\n"

/**
 * A layout for the tartarus
 */
struct layout {
	/**
	 * ID specified in config file
	 */
	guint id;

	/**
	 * Kep mapping
	 */
	GPtrArray *combos[15 + 4 + 2];
};

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
struct config {
	char *config_dir;

	GPtrArray *programs;

	struct {
		enum usb_backlight backlight;
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
