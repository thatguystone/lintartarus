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
#include <glib.h>
#include <sys/types.h>

/**
 * Global state that everyone can mutilate
 */
struct state {
	/**
	 * Index of the currently-running program. -1 if none are running.
	 */
	int progi;

	/**
	 * PID of the currently-running program
	 */
	pid_t prog_pid;

	/**
	 * Number of the current layout in use. 0 if no layout is in use.
	 */
	guint layout;
};

/**
 * Globally-usable variable
 */
struct state state;

/**
 * Yet another basic setup
 */
void state_init(void);

/**
 * Check if there have been any state changes
 */
gboolean state_has_changed(void);

/**
 * Set the global layout
 */
void state_set_layout(guint layout);

/**
 * Set the current running program
 */
void state_set_prog(int progi, pid_t pid);
