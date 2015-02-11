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

#include <errno.h>
#include <glib.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "config.h"
#include "poll.h"
#include "usb.h"
#include "x.h"

/**
 * Inotify fd for watching for config changes
 */
static int _ifd;

void poll_init()
{
	int err;

	_ifd = inotify_init1(IN_NONBLOCK);
	if (_ifd == -1) {
		g_error("failed to create inotify instance: %s", strerror(errno));
	}

	err = inotify_add_watch(_ifd, cfg.config_dir, IN_CLOSE_WRITE | IN_MOVED_TO);
	if (err == -1) {
		g_error("failed to watch config directory: %s", strerror(errno));
	}
}

void poll_run()
{
	int err;
	char buff[1024];
	struct pollfd fds[] = {
		{	.fd = _ifd,
			.events = POLLIN,
		},
		{	.fd = x_get_fd(),
			.events = POLLIN,
		},
	};

	err = poll(fds, G_N_ELEMENTS(fds), -1);
	if (err == -1) {
		return;
	}

	if (fds[0].revents & POLLIN) {
		// Don't care about what happened, just that something did
		while (read(_ifd, buff, sizeof(buff)) > 0);

		cfg_reload();
	}

	if (fds[1].revents & POLLIN) {
		x_poll();
	}
}
