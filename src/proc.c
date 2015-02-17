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
#include <glib.h>
#include <string.h>
#include "callbacks.h"
#include "config.h"
#include "proc.h"
#include "state.h"

static void _set_active(const int progi, const pid_t pid)
{
	if (progi == -1) {
		if (state.progi != -1) {
			g_debug("active program exited");
		}
	} else {
		struct program *prog = g_ptr_array_index(cfg.programs, progi);
		g_debug("setting active program to %s", prog->name);
	}

	state_set_prog(progi, pid);

	if (progi == -1) {
		cbs_prog_end();
	} else {
		cbs_prog_start();
	}
}

static gboolean _check_cmd(const char *cmd, const pid_t pid)
{
	guint i;
	guint j;
	char *patt;
	struct program *prog;

	for (i = 0; i < cfg.programs->len; i++) {
		prog = g_ptr_array_index(cfg.programs, i);

		for (j = 0; j < prog->cmds->len; j++) {
			patt = g_ptr_array_index(prog->cmds, j);
			if (strstr(cmd, patt) != NULL) {
				_set_active(i, pid);
				return TRUE;
			}
		}
	}

	return FALSE;
}

static gboolean _check_exec(const char *exe, const pid_t pid)
{
	guint i;
	guint j;
	char *patt;
	gboolean match;
	struct program *prog;

	for (i = 0; i < cfg.programs->len; i++) {
		prog = g_ptr_array_index(cfg.programs, i);

		for (j = 0; j < prog->exes->len; j++) {
			patt = g_ptr_array_index(prog->exes, j);

			if (*patt == '/') {
				match = g_strcmp0(exe, patt) == 0;
			} else {
				match = strstr(exe, patt) != NULL;
			}

			if (match) {
				_set_active(i, pid);
				return TRUE;
			}
		}
	}

	return FALSE;
}


static void _check_active(void)
{
	GDir *dir;
	char *end;
	pid_t pid;
	gboolean ok;
	char *contents;
	const char *path;
	gboolean running = FALSE;
	GString *buff = g_string_new("");

	dir = g_dir_open("/proc", 0, NULL);
	while ((path = g_dir_read_name(dir))) {
		pid = g_ascii_strtoull(path, &end, 10);

		// Directory isn't a number, so not a pid
		if (*end != '\0') {
			continue;
		}

		g_string_printf(buff, "/proc/%d/cmdline", pid);
		ok = g_file_get_contents(buff->str, &contents, NULL, NULL);
		if (ok && _check_cmd(contents, pid)) {
			running = TRUE;
			break;
		}

		g_free(contents);
		contents = NULL;

		g_string_printf(buff, "/proc/%d/exe", pid);
		contents = g_file_read_link(buff->str, NULL);
		if (contents != NULL && _check_exec(contents, pid)) {
			running = TRUE;
			break;
		}

		g_free(contents);
		contents = NULL;
	}

	if (!running) {
		_set_active(-1, -1);
	}

	g_free(contents);
	g_string_free(buff, TRUE);
	g_dir_close(dir);
}

void proc_on_poll_tick()
{
	int err;

	if (state.progi == -1) {
		_check_active();
	} else {
		err = kill(state.prog_pid, 0);
		if (err == -1 && errno == ESRCH) {
			_set_active(-1, -1);
		}
	}
}

void proc_on_config_updated()
{
	_check_active();
}
