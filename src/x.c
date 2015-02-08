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

int x()
{
	XEvent ev;
	Display* dpy = XOpenDisplay(0);
	Window root = DefaultRootWindow(dpy);

	unsigned int modifiers = ControlMask | ShiftMask;
	int keycode = XKeysymToKeycode(dpy,XK_K);
	Window grab_window =  root;
	int owner_events = 0;
	int pointer_mode = GrabModeAsync;
	int keyboard_mode = GrabModeAsync;

	XGrabKey(dpy, keycode, modifiers, grab_window, owner_events, pointer_mode, keyboard_mode);

	XSelectInput(dpy, root, KeyPressMask);
	while (1) {
		int shouldQuit = 0;
		XNextEvent(dpy, &ev);
		switch (ev.type) {
			case KeyPress:
				printf("Hot key pressed!\n");
				XUngrabKey(dpy,keycode,modifiers,grab_window);
				shouldQuit = 1;

			default:
				break;
		}

		if (shouldQuit) {
			break;
		}
	}

	XCloseDisplay(dpy);
	return 0;
}
