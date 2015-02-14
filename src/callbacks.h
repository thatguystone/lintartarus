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

/**
 * Poll completed a round, do some busy work
 */
void cbs_poll_tick(void);

/**
 * Notification of a configuration update
 */
void cbs_config_updated(void);

/**
 * A new program started
 */
void cbs_prog_start(void);

/**
 * A program ended
 */
void cbs_prog_end(void);

/**
 * Notification of a state change
 */
void cbs_state_changed(void);

/**
 * Change was (maybe?) updated
 */
void cbs_check_state(void);
