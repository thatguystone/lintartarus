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

#include <string.h>
#include "state.h"

static gboolean _changed;

void state_init()
{
	state_set_layout(0);
	state_set_prog(-1, -1);
}

gboolean state_has_changed()
{
	gboolean changed = _changed;

	_changed = FALSE;

	return changed;
}

void state_set_layout(guint layout)
{
	if (state.layout != layout) {
		state.layout = layout;
		_changed = TRUE;
	}
}

void state_set_prog(int progi, pid_t pid)
{
	if (progi != state.progi || pid != state.prog_pid) {
		state.progi = progi;
		state.prog_pid = pid;
		_changed = TRUE;
	}
}
