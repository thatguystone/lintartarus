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

#include <glib.h>
#include <stdio.h>
#include "proc.h"

static gboolean _check_cmd(const char *cmd G_GNUC_UNUSED)
{
	return FALSE;
}

static gboolean _check_exec(const char *bin G_GNUC_UNUSED)
{
	return FALSE;
}

void proc_sync()
{
	GDir *dir;
	char *end;
	pid_t pid;
	gboolean ok;
	char *contents;
	const char *path;
	GString *buff = g_string_new("");

	dir = g_dir_open("/proc", 0, NULL);
	while ((path = g_dir_read_name(dir))) {
		pid = g_ascii_strtoull(path, &end, 10);

		// Directory name not a number, so not a pid
		if (*end != '\0') {
			continue;
		}

		g_string_printf(buff, "/proc/%d/cmd", pid);
		ok = g_file_get_contents(buff->str, &contents, NULL, NULL);
		if (ok && _check_cmd(contents)) {
			break;
		}

		g_free(contents);
		contents = NULL;

		g_string_printf(buff, "/proc/%d/exe", pid);
		contents = g_file_read_link(buff->str, NULL);
		if (contents != NULL && _check_exec(contents)) {
			break;
		}

		g_free(contents);
		contents = NULL;
	}

	g_free(contents);
	g_string_free(buff, TRUE);
	g_dir_close(dir);
}
