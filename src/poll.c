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
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include "callbacks.h"
#include "poll.h"

static int _epoll;
static GHashTable *_cbs;

void poll_init()
{
	_cbs = g_hash_table_new(NULL, NULL);
	_epoll = epoll_create1(0);
	if (_epoll == -1) {
		g_error("failed to init epoll: %s", strerror(errno));
	}
}

void poll_mod(int fd, poll_cb cb, gboolean read, gboolean write)
{
	int err;
	struct epoll_event ev = {
		.events = 0,
		.data.fd = fd,
	};

	if (read) {
		ev.events |= EPOLLIN;
	}

	if (write) {
		ev.events |= EPOLLOUT;
	}

	if (!g_hash_table_contains(_cbs, GINT_TO_POINTER(fd))) {
		err = epoll_ctl(_epoll, EPOLL_CTL_ADD, fd, &ev);
		if (err == -1) {
			g_error("failed to add to epoll: %s", strerror(errno));
		}
	} else {
		err = epoll_ctl(_epoll, EPOLL_CTL_MOD, fd, &ev);
		if (err == -1) {
			g_error("failed to mod event: %s", strerror(errno));
		}
	}

	// Allow cb to be changed
	g_hash_table_insert(_cbs, GINT_TO_POINTER(fd), cb);
}

void poll_rm(int fd)
{
	g_hash_table_remove(_cbs, GINT_TO_POINTER(fd));
	epoll_ctl(_epoll, EPOLL_CTL_DEL, fd, NULL);
}

void poll_run()
{
	int i;
	int fd;
	int err;
	poll_cb cb;
	struct epoll_event evs[8];

	g_debug("poll running...");

	while (TRUE) {
		err = epoll_wait(_epoll, evs, G_N_ELEMENTS(evs), 1000);

		for (i = 0; i < err; i++) {
			fd = evs[i].data.fd;
			cb = g_hash_table_lookup(_cbs, GINT_TO_POINTER(fd));
			if (cb != NULL) {
				cb(fd);
			}
		}

		cbs_poll_tick();
	}
}
