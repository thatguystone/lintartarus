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
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "config.h"
#include "layout.h"
#include "proc.h"
#include "x.h"

struct _binding {
	char **cfg;
	guint modifiers;
	guint keycode;
	void (*cb)(void);
};

static Window _root;
static Display *_display;

static struct _binding _bindings[] = {
	{	.cfg = &cfg.hotkeys.launch,
		.cb = proc_sync,
	},
	{	.cfg = &cfg.hotkeys.next,
		.cb = layout_next,
	},
	{	.cfg = &cfg.hotkeys.prev,
		.cb = layout_prev,
	},
};

static void _parse_binding(const char *combo, struct _binding *b)
{
	char *k;
	char **iter;
	char **parts = g_strsplit(combo, "+", 0);

	b->modifiers = b->keycode = 0;

	iter = parts;
	while (*iter != NULL) {
		g_strstrip(*iter);
		k = g_utf8_strdown(*iter, -1);

		if (g_str_equal(k, "alt")) {
			b->modifiers |= Mod1Mask;
		} else if (g_str_equal(k, "super")) {
			b->modifiers |= Mod4Mask;
		} else if (g_str_equal(k, "ctrl")) {
			b->modifiers |= ControlMask;
		} else if (g_str_equal(k, "shift")) {
			b->modifiers |= ShiftMask;
		} else if (strlen(k) == 1) {
			b->keycode = XKeysymToKeycode(_display, XStringToKeysym(k));
		} else {
			g_critical("ignoring unknown key: %s", k);
		}

		g_free(k);
		iter++;
	}

	g_strfreev(parts);
}

void x_init()
{
	_display = XOpenDisplay(0);
	_root = DefaultRootWindow(_display);
	XSelectInput(_display, _root, KeyPressMask);
}

void x_sync()
{
	guint i;

	for (i = 0; i < G_N_ELEMENTS(_bindings); i++) {
		struct _binding *b = &_bindings[i];

		XUngrabKey(_display, b->keycode, b->modifiers, _root);

		_parse_binding(*b->cfg, b);

		XGrabKey(_display,
			b->keycode, b->modifiers,
			_root, 0,
			GrabModeAsync, GrabModeAsync);
	}

	// For some reason, without this, poll never fires...
	XPending(_display);
}

void x_poll()
{
	guint i;
	union {
		XEvent ev;
		XKeyEvent kev;
	} xev;

	while (XPending(_display)) {
		XNextEvent(_display, &xev.ev);
		switch (xev.ev.type) {
			case KeyPress:
				for (i = 0; i < G_N_ELEMENTS(_bindings); i++) {
					struct _binding *b = &_bindings[i];
					if (xev.kev.keycode == b->keycode &&
						xev.kev.state == b->modifiers) {

						b->cb();
					}
				}
				break;

			default:
				break;
		}
	}
}

int x_fd()
{
	return XConnectionNumber(_display);
}
