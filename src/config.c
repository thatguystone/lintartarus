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
#include <sys/inotify.h>
#include <unistd.h>
#include <wordexp.h>
#include "config.h"
#include "udev.h"
#include "usb.h"
#include "usb_dump.h"

#define INDENT "    "

/**
 * Inotify fd for watching for config changes
 */
static int _ifd;

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
	_print_opt("aGROUP", "authorize=GROUP", "add a udev rule to allow the given group to access the device without root");
	_print_opt("cDIR", "config-dir=DIR", "directory to use for config files (~/.config/lintartarus)");
	_print_opt("d", "daemonize", "run in the background as a daemon");
	_print_opt(NULL, "dump-config", "dump parse config values");
	_print_opt(NULL, "dump-usb", "dump USB debugging info");
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

static int _strcmp(const void *a_, const void *b_)
{
	const char * const *a = a_;
	const char * const *b = b_;
	return g_strcmp0(*a, *b);
}

static void _layout_free(void *l_)
{
	guint i;
	struct layout *l = l_;

	for (i = 0; i < G_N_ELEMENTS(l->keys); i++) {
		g_free(l->keys[i]);
	}

	g_free(l);
}

static struct layout* _layout_new(void)
{
	guint i;
	struct layout *l = g_malloc0(sizeof(*l));

	for (i = 0; i < G_N_ELEMENTS(l->keys); i++) {
		l->keys[i] = layout_get_default(i);
	}

	return l;
}

static int _layout_cmp(const void *a_, const void *b_)
{
	const struct layout * const *a = a_;
	const struct layout * const *b = b_;
	return (*a)->id - (*b)->id;
}

static void _program_free(void *prog_)
{
	struct program *prog = prog_;
	g_ptr_array_free(prog->cmds, TRUE);
	g_ptr_array_free(prog->exes, TRUE);
	g_ptr_array_free(prog->layouts, TRUE);
	g_free(prog->name);
	g_free(prog);
}

static int _program_cmp(const void *a_, const void *b_)
{
	const struct program * const *a = a_;
	const struct program * const *b = b_;
	return g_strcmp0((*a)->name, (*b)->name);
}

static void _parse_program(
	struct program *prog G_GNUC_UNUSED,
	GKeyFile *kf,
	const char *group_name)
{
	size_t i;
	char *val;
	char **keys;
	GPtrArray *ary;
	size_t len = 0;

	keys = g_key_file_get_keys(kf, group_name, &len, NULL);
	for (i = 0; i < len; i++) {
		ary = NULL;
		if (g_str_has_prefix(keys[i], "cmd")) {
			ary = prog->cmds;
		} else if (g_str_has_prefix(keys[i], "exe")) {
			ary = prog->exes;
		}

		if (ary != NULL) {
			val = g_key_file_get_string(kf, group_name, keys[i], NULL);
			g_ptr_array_add(ary, g_strdup(val));
		}
	}

	g_strfreev(keys);
}

static void _parse_layout(
	struct program *prog,
	GKeyFile *kf,
	const char *group_name,
	const char *main_group,
	const char *layout_id)
{
	guint i;
	guint id;
	char *val;
	char *end;
	struct layout *l;

	id = g_ascii_strtoull(layout_id, &end, 10);
	if (*end != '\0') {
		g_critical("ignoring invalid layout id for %s: %s",
			main_group,
			layout_id);
		return;
	}

	l = _layout_new();
	l->id = id;

	for (i = 0; i < G_N_ELEMENTS(l->keys); i++) {
		val = g_key_file_get_string(kf, group_name, layout_get_name(i), NULL);
		if (val != NULL) {
			g_free(l->keys[i]);
			l->keys[i] = val;
		}
	}

	g_ptr_array_add(prog->layouts, l);
}

static void _build_progs(GKeyFile *kf)
{
	size_t i;
	size_t j;
	char **parts;
	char **groups;
	size_t groupsc;
	struct program *prog;

	if (cfg.programs != NULL) {
		g_ptr_array_free(cfg.programs, TRUE);
	}

	cfg.programs = g_ptr_array_new_with_free_func(_program_free);

	groups = g_key_file_get_groups(kf, &groupsc);
	for (i = 0; i < groupsc; i++) {
		if (g_str_equal(groups[i], "default")) {
			continue;
		}

		parts = g_strsplit(groups[i], ":", 0);
		prog = NULL;
		for (j = 0; j < cfg.programs->len; j++) {
			struct program *p = g_ptr_array_index(cfg.programs, j);
			if (g_str_equal(p->name, parts[0])) {
				prog = p;
				break;
			}
		}

		if (prog == NULL) {
			prog = g_malloc0(sizeof(*prog));
			prog->name = g_strdup(parts[0]);
			prog->cmds = g_ptr_array_new_with_free_func(g_free);
			prog->exes = g_ptr_array_new_with_free_func(g_free);
			prog->layouts = g_ptr_array_new_with_free_func(_layout_free);
			g_ptr_array_add(cfg.programs, prog);
		}

		if (g_strv_length(parts) == 1) {
			_parse_program(prog, kf, parts[0]);
		} else {
			_parse_layout(prog, kf, groups[i], parts[0], parts[1]);
		}

		g_strfreev(parts);
	}

	g_strfreev(groups);

	g_ptr_array_sort(cfg.programs, _program_cmp);
	for (i = 0; i < cfg.programs->len; i++) {
		prog = g_ptr_array_index(cfg.programs, i);
		g_ptr_array_sort(prog->cmds, _strcmp);
		g_ptr_array_sort(prog->exes, _strcmp);
		g_ptr_array_sort(prog->layouts, _layout_cmp);
	}
}

void cfg_init(int argc, char **argv)
{
	int err;
	gboolean dump_cfg = FALSE;
	struct option lopts[] = {
		{ "authorize", optional_argument, NULL, 'a' },
		{ "config-dir", required_argument, NULL, 'c' },
		{ "daemonize", no_argument, NULL, 'd' },
		{ "dump-config", no_argument, NULL, '\1' },
		{ "dump-usb", no_argument, NULL, '\2' },
		{ "help", no_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 },
	};

	memset(&cfg, 0, sizeof(cfg));

	while (1) {
		char c = getopt_long(argc, argv, "a::c:dh", lopts, NULL);
		if (c == -1) {
			break;
		}

		switch (c) {
			case '\1':
				dump_cfg = TRUE;
				break;

			case '\2':
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

	_ifd = inotify_init1(IN_NONBLOCK);
	if (_ifd == -1) {
		g_error("failed to create inotify instance: %s", strerror(errno));
	}

	cfg_reload();

	err = inotify_add_watch(_ifd, cfg.config_dir, IN_CLOSE_WRITE | IN_MOVED_TO);
	if (err == -1) {
		g_error("failed to watch config directory: %s", strerror(errno));
	}

	if (dump_cfg) {
		cfg_dump();
		exit(0);
	}
}

static const char* _brightness_str(int brightness)
{
	switch (brightness) {
		case bright_off:  return "off";
		case bright_low:  return "low";
		case bright_med:  return "med";
		case bright_high: return "high";
		default:          return "unknown";
	}
}

void cfg_dump(void)
{
	guint i;
	guint j;
	guint k;

	printf("config dir: %s\n", cfg.config_dir);
	printf("pulse: %s\n", cfg.usb.pulse ? "true" : "false");
	printf("brightness: %s\n", _brightness_str(cfg.usb.brightness));
	printf("\n");
	printf("hotkeys:\n");
	printf(INDENT "launch:      %s\n", cfg.hotkeys.launch);
	printf(INDENT "next-layout: %s\n", cfg.hotkeys.next);
	printf(INDENT "prev-layout: %s\n", cfg.hotkeys.prev);
	printf("\n");
	printf("programs (%u):\n", cfg.programs->len);

	for (i = 0; i < cfg.programs->len; i++) {
		struct program *prog = g_ptr_array_index(cfg.programs, i);

		printf(INDENT "%s (layouts: %u):\n", prog->name, prog->layouts->len);

		printf(INDENT INDENT "cmds (%u):\n", prog->cmds->len);
		for (j = 0; j < prog->cmds->len; j++) {
			char *s = g_ptr_array_index(prog->cmds, j);
			printf(INDENT INDENT INDENT "%s\n", s);
		}

		printf(INDENT INDENT "exes (%u):\n", prog->exes->len);
		for (j = 0; j < prog->exes->len; j++) {
			char *s = g_ptr_array_index(prog->exes, j);
			printf(INDENT INDENT INDENT "%s: %s\n",
				*s == '/' ? "abs" : "rel",
				s);
		}

		for (j = 0; j < prog->layouts->len; j++) {
			struct layout *l = g_ptr_array_index(prog->layouts, j);

			printf(INDENT INDENT "layout %u:\n", j + 1);

			for (k = 0; k < G_N_ELEMENTS(l->keys); k++) {
				printf(INDENT INDENT INDENT "%10s => %s\n",
					layout_get_name(k),
					l->keys[k]);
			}
		}
	}

	printf("\n");
}

int cfg_fd()
{
	return _ifd;
}

void cfg_reload()
{
	int err;
	GDir *dir;
	GKeyFile *kf;
	gboolean ok;
	char *contents;
	char ifdbuf[1024];
	const char *path;
	char *brightness;
	GError *error = NULL;
	GString *buff = g_string_new("");
	GString *config = g_string_new("");

	// Don't care about what happened, just that something did
	while (read(_ifd, ifdbuf, sizeof(ifdbuf)) > 0);

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

	dir = g_dir_open(cfg.config_dir, 0, NULL);
	while ((path = g_dir_read_name(dir))) {
		g_string_printf(buff, "%s/%s", cfg.config_dir, path);
		ok = g_file_get_contents(buff->str, &contents, NULL, &error);

		if (ok) {
			g_string_append_c(config, '\n');
			g_string_append(config, contents);
			g_free(contents);
		} else if (error->code != G_FILE_ERROR_NOENT) {
			g_critical("failed to open config \"%s\": %s\n",
				buff->str,
				error->message);
		}

		g_clear_error(&error);
	}

	kf = g_key_file_new();
	ok = g_key_file_load_from_data(kf, config->str, config->len, 0, &error);
	if (!ok) {
		g_critical("failed to parse config: %s\n", error->message);
		g_clear_error(&error);
	}

	cfg.usb.pulse = g_key_file_get_boolean(kf, "default", "pulse", &error);
	if (error != NULL) {
		g_critical("invalid pulse config, defaulting to false");
		g_clear_error(&error);
	}

	brightness = g_key_file_get_string(kf, "default", "brightness", NULL);
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

	_key_layout(kf, "launch", &cfg.hotkeys.launch, "ctrl+alt+shift+l");
	_key_layout(kf, "next-layout", &cfg.hotkeys.next, "ctrl+alt+shift+n");
	_key_layout(kf, "prev-layout", &cfg.hotkeys.prev, "ctrl+alt+shift+p");

	_build_progs(kf);

	g_free(brightness);
	g_dir_close(dir);
	g_key_file_free(kf);
	g_string_free(buff, TRUE);
	g_string_free(config, TRUE);

	usb_sync();
}
