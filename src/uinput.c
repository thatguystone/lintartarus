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
#include <fcntl.h>
#include <glib.h>
#include <linux/uinput.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "callbacks.h"
#include "const.h"
#include "keys.h"
#include "layout.h"
#include "poll.h"
#include "uinput.h"

#define INPUT_DIR "/dev/input"

static const char *_uinput_paths[] = {
	"/dev/uinput",
	"/dev/input/uinput",
	"/dev/misc/uinput",
};

/**
 * Created output keyboard
 */
static int _out;

/**
 * Inotify fd for watching for device changes
 */
static int _ifd;

/**
 * All FDs to watch for events
 */
static GArray *_fds;

static void _dev_close(void *fd_)
{
	int *fd = fd_;
	ioctl(*fd, EVIOCGRAB, 0);
	close(*fd);
	poll_rm(*fd);
}

static void _send_syn(void)
{
	int err;
	struct input_event ev = {
		.type = EV_SYN,
	};

	err = write(_out, &ev, sizeof(ev));
	if (err != sizeof(ev)) {
		g_error("failed to send EV_SYN: %s", strerror(errno));
	}
}

static void _handle_code(int code, int value)
{
	int err;
	struct input_event ev = {
		.type = EV_KEY,
		.value = value,
		.code = code,
	};

	if (code < 0) {
		layout_handle_internal(code);
	} else {
		err = write(_out, &ev, sizeof(ev));
		if (err != sizeof(ev)) {
			g_error("failed to send key code %d: %s", code, strerror(errno));
		}

		_send_syn();
	}
}

static void _event(int fd)
{
	guint i;
	guint j;
	ssize_t err;
	GArray *seq;
	struct input_event ev;
	const GPtrArray *combo;

	err = read(fd, &ev, sizeof(ev));
	if (err != sizeof(ev)) {
		if (err != -1) {
			g_critical("did not get complete input event: "
				"%" G_GSSIZE_FORMAT " != %" G_GSSIZE_FORMAT,
				err,
				sizeof(ev));
		}

		return;
	}

	if (ev.type != EV_KEY) {
		return;
	}

	combo = layout_translate(ev.code);
	if (combo == NULL) {
		return;
	}

	g_debug("got combo");

	/*
	 * If there's only 1 combo, fire it in step with the key press from the
	 * device. If there's more than 1, fire the combo and ignore keyup.
	 */
	if (combo->len == 1) {
		seq = g_ptr_array_index(combo, 0);
		for (i = 0; i < seq->len; i++) {
			_handle_code(g_array_index(seq, int, i), ev.value);
		}
	} else if (ev.value == 1) {
		for (i = 0; i < combo->len; i++) {
			seq = g_ptr_array_index(combo, i);

			// Send key down
			for (j = 0; j < seq->len; j++) {
				_handle_code(g_array_index(seq, int, i), 1);
			}

			// Send key up
			for (j = 0; j < seq->len; j++) {
				_handle_code(g_array_index(seq, int, i), 0);
			}
		}
	}
}

static void _sync_devs(int fd)
{
	int err;
	GDir *dir;
	const char *path;
	char ifdbuf[1024];
	struct input_id info;
	GString *buff = g_string_new("");

	g_array_set_size(_fds, 0);

	// Don't care about what happened, just that something did
	while (read(fd, ifdbuf, sizeof(ifdbuf)) > 0);

	dir = g_dir_open(INPUT_DIR, 0, NULL);
	while ((path = g_dir_read_name(dir))) {
		g_string_printf(buff, "%s/%s", INPUT_DIR, path);
		if (g_file_test(buff->str, G_FILE_TEST_IS_DIR)) {
			continue;
		}

		fd = open(buff->str, O_RDONLY);
		if (fd == -1) {
			goto end;
		}

		err = ioctl(fd, EVIOCGID, &info);
		if (err == -1) {
			goto end;
		}

		if (info.vendor != VENDOR || info.product != PRODUCT) {
			goto end;
		}

		err = ioctl(fd, EVIOCGRAB, 1);
		if (err == -1) {
			goto end;
		}

		err = fcntl(fd, F_SETFL, O_NONBLOCK);
		if (err == -1) {
			goto end;
		}

		poll_mod(fd, _event, TRUE, FALSE);
		g_array_append_val(_fds, fd);
		continue;

end:
		if (fd != -1) {
			close(fd);
		}
	}

	g_string_free(buff, TRUE);
	g_dir_close(dir);
}

void uinput_init(void)
{
	int fd;
	int ifd;
	int err;
	guint i;
	int code;
	GArray *codes = keys_get_all_codes();
	struct uinput_user_dev uidev = {
		.name = "lintartarus keyboard",
		.id = {
			.bustype = BUS_VIRTUAL,
			.vendor = 0x1337,
			.product = 0xb33f,
			.version = 1,
		},
	};

	_fds = g_array_new(FALSE, FALSE, sizeof(int));
	g_array_set_clear_func(_fds, _dev_close);

	fd = -1;
	for (i = 0; fd == -1 && i < G_N_ELEMENTS(_uinput_paths); i++) {
		fd = open(_uinput_paths[i], O_WRONLY | O_NONBLOCK);
		if (fd == -1) {
			if (errno == EACCES) {
				g_error("failed to open uinput device: %s", strerror(errno));
			}
		}
	}

	if (fd == -1) {
		g_error("uinput device not found. did you `modprobe uinput`?");
	}

	err = write(fd, &uidev, sizeof(uidev));
	if (err == -1) {
		g_error("failed to init uinput keyboard: %s", strerror(errno));
	}

	err = ioctl(fd, UI_SET_EVBIT, EV_KEY);
	if (err == -1) {
		g_error("failed to setup uinput keyboard: %s", strerror(errno));
	}

	err = ioctl(fd, UI_SET_EVBIT, EV_SYN);
	if (err == -1) {
		g_error("failed to setup uinput keyboard: %s", strerror(errno));
	}

	for (i = 0; i < codes->len; i++) {
		code = g_array_index(codes, int, i);

		// Internal code, can't be fired
		if (code < 0) {
			continue;
		}

		err = ioctl(fd, UI_SET_KEYBIT, code);
		if (err == -1) {
			g_error("failed to setup key %s: %s",
				keys_val(code),
				strerror(errno));
		}
	}

	g_array_free(codes, TRUE);

	err = ioctl(fd, UI_DEV_CREATE);
	if (err == -1) {
		g_error("failed to create uinput keyboard: %s", strerror(errno));
	}

	_out = fd;

	ifd = inotify_init1(IN_NONBLOCK);
	if (_ifd == -1) {
		g_error("failed to create inotify input instance: %s",
			strerror(errno));
	}

	err = inotify_add_watch(ifd, INPUT_DIR, IN_CREATE | IN_DELETE | IN_ATTRIB);
	if (err == -1) {
		g_error("failed to watch input directory: %s",
			strerror(errno));
	}

	poll_mod(ifd, _sync_devs, TRUE, FALSE);
	_sync_devs(ifd);
}
