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
#include <libusb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
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
	const guint static_events = 2;

	guint i;
	int err;
	guint usb_fdsc;
	char buff[1024];
	gboolean poll_usb = FALSE;
	const struct libusb_pollfd **usb_fds = usb_get_pollfds(&usb_fdsc);
	struct pollfd fds[static_events + usb_fdsc];

	fds[0].fd = _ifd;
	fds[0].events = POLLIN;

	fds[1].fd = x_get_fd();
	fds[1].events = POLLIN;

	for (i = 0; i < usb_fdsc; i++) {
		fds[static_events + i].fd = usb_fds[i]->fd;
		fds[static_events + i].events = usb_fds[i]->events;
	}

	free(usb_fds);

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

	for (i = 0; i < usb_fdsc; i++) {
		poll_usb |= fds[static_events + i].revents != 0;
	}

	if (poll_usb) {
		usb_async_poll();
	}
}
