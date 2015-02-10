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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "poll.h"
#include "usb.h"
#include "usb_dump.h"
#include "util.h"
#include "x.h"

#define INDENT "    "

static void _print_opt(const char *s, const char *arg, const char *desc)
{
	printf(INDENT "-%s, --%s\n", s, arg);
	printf(INDENT INDENT "%s\n", desc);
}

static void _print_usage(char **argv)
{
	printf("Usage: %s [OPTION]...\n", argv[0]);
	printf("\n");
	_print_opt("a", "authorize", "add a udev rule to allow device access without root");
	_print_opt("b", "brightness", "adjust backlight brightness (opts: off, low, med, high)");
	_print_opt("d", "dump", "dump USB debugging info");
	_print_opt("h", "help", "print this message");
	_print_opt("p", "pulse", "pulse (opts: on, off)");

	exit(2);
}

static void _getopt(int argc, char **argv)
{
	struct option lopts[] = {
		{ "authorize", no_argument, NULL, 'a' },
		{ "brightness", required_argument, NULL, 'b' },
		{ "dump", no_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
		{ "pulse", required_argument, NULL, 'p' },
	};

	while (1) {
		char c = getopt_long(argc, argv, "a:b:dhp:", lopts, NULL);
		if (c == -1) {
			break;
		}

		switch (c) {
			case 'a':
				break;

			case 'd':
				usb_dump();
				exit(0);
				break;

			case 'b':
				if (strcmp(optarg, "off") == 0) {
					usb_set_brightness(bright_off);
				} else if (strcmp(optarg, "low") == 0) {
					usb_set_brightness(bright_low);
				} else if (strcmp(optarg, "med") == 0) {
					usb_set_brightness(bright_med);
				} else if (strcmp(optarg, "high") == 0) {
					usb_set_brightness(bright_high);
				} else {
					fprintf(stderr, "invalid brightness argument: %s\n", optarg);
					_print_usage(argv);
				}

				break;

			case 'p':
				if (strcmp(optarg, "on") == 0) {
					usb_set_pulse(1);
				} else if (strcmp(optarg, "off") == 0) {
					usb_set_pulse(0);
				} else {
					fprintf(stderr, "invalid pulse argument: %s\n", optarg);
					_print_usage(argv);
				}

				break;

			case 'h':
			default:
				_print_usage(argv);
		}
	}
}

int main(int argc, char **argv)
{
	usb_init();
	_getopt(argc, argv);

	x_init();
	while (1) {
		poll_run();
	}

	return 0;
}
