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
#include <getopt.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wordexp.h>
#include "config.h"
#include "udev.h"
#include "usb.h"
#include "usb_dump.h"
#include "x.h"

#define INDENT "    "

static void _print_opt(const char *s, const char *arg, const char *desc)
{
	printf(INDENT);

	if (s != NULL) {
		printf("-%s, ", s);
	}

	printf("--%s\n", arg);
	printf(INDENT INDENT "%s\n", desc);
}

static void _print_usage(char **argv)
{
	printf("Usage: %s [OPTION]...\n", argv[0]);
	printf("\n");
	_print_opt("a GROUP", "authorize=GROUP", "add a udev rule to allow the given group to access the device without root");
	_print_opt("c DIR", "config-dir=DIR", "directory to use for config files (~/.config/lintartarus)");
	_print_opt("d", "daemonize", "run in the background as a daemon");
	_print_opt(NULL, "dump", "dump USB debugging info");
	_print_opt("h", "help", "print this message");

	exit(2);
}

static void _set_config_dir(const char *dir)
{
	int err;
	wordexp_t p;

	err = wordexp(dir, &p, WRDE_UNDEF);
	if (err != 0) {
		const char *strerr;
		switch (err) {
			case WRDE_BADCHAR:
				strerr = "Illegal occurrence of newline or one of "
					"|, &, ;, <, >, (, ), {, }.";
				break;
			case WRDE_BADVAL:
				strerr = "An undefined shell variable was referenced";
				break;
			case WRDE_NOSPACE:
				strerr = "Out of memory";
				break;
			case WRDE_SYNTAX:
				strerr = "Syntax error in expansion";
				break;
			default:
				strerr = "unknown";
				break;
		}

		g_error("invalid config directory: %s\n", strerr);
	}

	cfg.config_dir = g_strdup(p.we_wordv[0]);
	wordfree(&p);
}

static void _key_layout(
	GKeyFile *f,
	const char *key,
	char **to,
	const char *def)
{
	g_free(*to);
	*to = g_key_file_get_string(f, "default", key, NULL);
	if (*to == NULL) {
		*to = g_strdup(def);
	}
}

void cfg_init(int argc, char **argv)
{
	struct option lopts[] = {
		{ "authorize", required_argument, NULL, 'a' },
		{ "config-dir", required_argument, NULL, 'c' },
		{ "daemonize", no_argument, NULL, 'd' },
		{ "dump", no_argument, NULL, '\1' },
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 },
	};

	memset(&cfg, 0, sizeof(cfg));

	while (1) {
		char c = getopt_long(argc, argv, "a:c:dh", lopts, NULL);
		if (c == -1) {
			break;
		}

		switch (c) {
			case '\1':
				usb_dump();
				exit(0);
				break;

			case 'a':
				udev_authorize(optarg);
				break;

			case 'c':
				_set_config_dir(optarg);
				break;

			case 'd':
				break;

			case 'h':
			default:
				_print_usage(argv);
		}
	}

	if (cfg.config_dir == NULL) {
		_set_config_dir("~/.config/lintartarus");
	}

	cfg.layout = 8;
	cfg_reload();
}

void cfg_reload()
{
	int err;
	GDir *dir;
	GKeyFile *f;
	gboolean ok;
	const char *path;
	char *brightness;
	GError *error = NULL;
	GString *buff = g_string_new("");

	if (!g_file_test(cfg.config_dir, G_FILE_TEST_IS_DIR)) {
		err = g_mkdir_with_parents(cfg.config_dir, 0700);
		if (err != 0) {
			g_error("failed to create %s: %s",
				cfg.config_dir,
				strerror(errno));
		}
	}

	g_string_printf(buff, "%s/%s", cfg.config_dir, "config");
	if (!g_file_test(buff->str, G_FILE_TEST_EXISTS)) {
		ok = g_file_set_contents(buff->str, DEFAULT_CONFIG, -1, &error);
		if (!ok) {
			g_error("failed to create default config: %s", error->message);
		}
	}

	f = g_key_file_new();
	dir = g_dir_open(cfg.config_dir, 0, NULL);
	while ((path = g_dir_read_name(dir))) {
		g_string_printf(buff, "%s/%s", cfg.config_dir, path);
		ok = g_key_file_load_from_file(f, buff->str, 0, &error);
		if (!ok && error->code != G_FILE_ERROR_NOENT) {
			g_critical("failed to open config \"%s\": %s\n",
				buff->str,
				error->message);
		}
		g_clear_error(&error);
	}

	cfg.usb.pulse = g_key_file_get_boolean(f, "default", "pulse", &error);
	if (error != NULL) {
		g_critical("invalid pulse config, defaulting to false");
		g_clear_error(&error);
	}

	brightness = g_key_file_get_string(f, "default", "brightness", NULL);
	if (g_strcmp0(brightness, "off") == 0) {
		cfg.usb.brightness = bright_off;
	} else if (g_strcmp0(brightness, "low") == 0) {
		cfg.usb.brightness = bright_low;
	} else if (g_strcmp0(brightness, "med") == 0) {
		cfg.usb.brightness = bright_med;
	} else if (g_strcmp0(brightness, "high") == 0) {
		cfg.usb.brightness = bright_high;
	} else {
		cfg.usb.brightness = bright_low;
		g_critical("invalid brightness config, defaulting to low");
	}

	_key_layout(f, "next-layout", &cfg.hotkeys.next, "ctrl+alt+shift+n");
	_key_layout(f, "prev-layout", &cfg.hotkeys.prev, "ctrl+alt+shift+p");

	g_free(brightness);
	g_dir_close(dir);
	g_key_file_free(f);
	g_string_free(buff, TRUE);

	x_sync();
	usb_sync();
}
