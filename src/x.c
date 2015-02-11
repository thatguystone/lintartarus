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
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "config.h"
#include "x.h"

struct _binding {
	guint modifiers;
	int keycode;
};

static Window _root;
static Display *_display;

static struct _binding _layout_next;
static struct _binding _layout_prev;

static void _parse_binding(const char *combo, struct _binding *b)
{
	char *k;
	char **iter;
	char **parts = g_strsplit(combo, "+", 0);

	memset(b, 0, sizeof(*b));

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
			g_error("unknown key: %s", k);
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
	XUngrabKey(_display,
		_layout_next.keycode, _layout_next.modifiers,
		_root);
	XUngrabKey(_display,
		_layout_prev.keycode, _layout_prev.modifiers,
		_root);

	_parse_binding(cfg.hotkeys.next, &_layout_next);
	_parse_binding(cfg.hotkeys.prev, &_layout_prev);

	XGrabKey(_display,
		_layout_next.keycode, _layout_next.modifiers,
		_root, 0,
		GrabModeAsync, GrabModeAsync);
	XGrabKey(_display,
		_layout_prev.keycode, _layout_prev.modifiers,
		_root, 0,
		GrabModeAsync, GrabModeAsync);

	// For some reason, without this, poll never fires...
	XPending(_display);
}

void x_poll()
{
	XEvent ev;

	while (XPending(_display)) {
		XNextEvent(_display, &ev);
		switch (ev.type) {
			case KeyPress:
				printf("Hot key pressed!\n");
				break;

			default:
				break;
		}
	}
}

int x_get_fd()
{
	return XConnectionNumber(_display);
}
