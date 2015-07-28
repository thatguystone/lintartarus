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

#include "callbacks.h"
#include "layout.h"
#include "proc.h"
#include "state.h"
#include "usb.h"

static void _state_changed(void)
{
	usb_on_state_changed();
}

void cbs_poll_tick()
{
	proc_on_poll_tick();
	usb_on_poll_tick();

	cbs_check_state();
}

void cbs_config_updated()
{
	proc_on_config_updated();
	layout_on_config_updated();

	// Ignore current state, possible for config to have changed things, so
	// sync anyway
	state_has_changed();
	_state_changed();
}

void cbs_prog_start()
{
	layout_on_prog_start();

	cbs_check_state();
}

void cbs_prog_end()
{
	layout_on_prog_end();

	cbs_check_state();
}

void cbs_check_state()
{
	if (!state_has_changed()) {
		return;
	}

	_state_changed();
}
