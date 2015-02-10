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

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "x.h"

static Window _root;
static Display *_display;

void x_init()
{
	_display = XOpenDisplay(0);
	_root = DefaultRootWindow(_display);
}

int x_poll()
{
	XEvent ev;


	unsigned int modifiers = ControlMask | ShiftMask;
	int keycode = XKeysymToKeycode(_display, XK_K);
	Window grab_window = _root;
	int owner_events = 0;
	int pointer_mode = GrabModeAsync;
	int keyboard_mode = GrabModeAsync;

	XGrabKey(_display, keycode, modifiers, grab_window, owner_events, pointer_mode, keyboard_mode);

	XSelectInput(_display, _root, KeyPressMask);
	while (1) {
		int shouldQuit = 0;
// #error http://stackoverflow.com/questions/14187135/xlib-keyboard-polling

		XNextEvent(_display, &ev);
		switch (ev.type) {
			case KeyPress:
				printf("Hot key pressed!\n");
				XUngrabKey(_display,keycode,modifiers,grab_window);
				shouldQuit = 1;

			default:
				break;
		}

		if (shouldQuit) {
			break;
		}
	}

	XCloseDisplay(_display);
	return 0;
}

int x_get_fd()
{
	return XConnectionNumber(_display);
}
