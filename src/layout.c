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
#include "config.h"
#include "layout.h"
#include "keys.h"
#include "state.h"

/**
 * Keycodes for input values, living at their index in layout.keys
 */
static int _mapping[G_N_ELEMENTS(((struct layout*)(NULL))->combos)];

void layout_init()
{
	guint i;

	for (i = 0; i < G_N_ELEMENTS(_mapping); i++) {
		_mapping[i] = keys_code(keys_get_dev_default(i));
	}
}

const GPtrArray* layout_translate(int code)
{
	guint i;
	gboolean found;
	struct layout *layout;
	struct program *program;

	if (state.layout == 0) {
		return NULL;
	}

	found = FALSE;
	for (i = 0; i < G_N_ELEMENTS(_mapping); i++) {
		if (_mapping[i] == code) {
			found = TRUE;
			break;
		}
	}

	if (!found) {
		return NULL;
	}

	program = g_ptr_array_index(cfg.programs, state.progi);
	layout = g_ptr_array_index(program->layouts, state.layout - 1);

	return layout->combos[i];
}

void layout_handle_internal(int code)
{
	guint layout = state.layout;
	struct program *program = g_ptr_array_index(cfg.programs, state.progi);

	switch (code) {
		case KEY_NEXT_LAYOUT:
			if (layout == program->layouts->len) {
				layout = 1;
			} else {
				layout++;
			}
			break;

		case KEY_PREV_LAYOUT:
			if (layout == 1) {
				layout = program->layouts->len;
			} else {
				layout--;
			}
			break;
	}

	state_set_layout(layout);
	cbs_check_state();
}

void layout_on_config_updated()
{
	struct program *program;

	if (state.layout == 0) {
		return;
	}

	program = g_ptr_array_index(cfg.programs, state.progi);

	state_set_layout(MIN(state.layout, program->layouts->len));
}

void layout_on_prog_start()
{
	if (state.layout == 0) {
		state_set_layout(1);
	}
}

void layout_on_prog_end()
{
	state_set_layout(0);
}
