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
#include <stdint.h>

/**
 * For looking up the device
 */
#define VENDOR UINT16_C(0x1532)
#define PRODUCT UINT16_C(0x0201)

/**
 * For sending lighting commands to the device
 */
#define wVALUE 0x0300
#define wINDEX 2

/**
 * Per the sepc, bits 0:3 of bEndpointAddress are the addr
 */
#define USB_EP_ADDR_MASK 0x0f

/**
 * Per the sepc, bit 7 of bEndpointAddress is the direction. Out is 0. In is 1.
 */
#define USB_EP_DIR_MASK 0x80

/**
 * Per the sepc, bits 0:1 of bmAttributes are the transfer type
 */
#define USB_EP_TYPE_MASK 0x03

/**
 * Each light-level command has two different values to send
 */
struct light_val {
	int a;
	int b;
};

/**
 * Each layout is composed of 3 different light values. Wat...
 */
struct layout_val {
	struct light_val a;
	struct light_val b;
	struct light_val c;
};

/**
 * How long each command is
 */
#define CMD_LEN 90

/**
 * Maximum number of commands that can be sent
 */
#define CMDS_MAX 5

/**
 * Where to place the first argument in command arrays
 */
#define ARG1I 10

/**
 * Where to place the second argument in command arrays
 */
#define ARG2I 88

/**
 * Command to set the light level
 */
const char *light_level_cmd;

/**
 * Command to change pulsate flag
 */
const char *pulsate_cmd;

/**
 * Commands to change the layout lights
 */
const char *layout_cmds[3];

/**
 * Values to set in a light-level request
 */
const struct light_val light_levels[4];

/**
 * Values to set in a pulsate request
 */
const struct light_val pulsate_vals[2];

/**
 * Values to set for changing the layout lights. These appear to be bitmasks
 * (spread across 6 bytes each), but they're defined in a different order.
 * This order is consistent with the windows util.
 */
const struct layout_val layout_vals[8];
