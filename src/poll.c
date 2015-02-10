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

#include <poll.h>
#include <stdio.h>
#include "poll.h"
#include "usb.h"
#include "x.h"

void poll_run()
{
	int err;
	int timeout = 1000;
	struct pollfd fds[] = {
		{	.fd = x_get_fd(),
			.events = POLLIN,
		},
	};

	usb_poll();
	if (!usb_connected()) {
		timeout /= 10;
	}

	err = poll(fds, sizeof(fds) / sizeof(*fds), timeout);
	if (err == -1) {
		return;
	}
}
