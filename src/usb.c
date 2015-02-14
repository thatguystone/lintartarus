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
#include <libusb.h>
#include <poll.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "const.h"
#include "poll.h"
#include "state.h"
#include "usb.h"

#define bmREQUEST_OUT \
	(LIBUSB_ENDPOINT_OUT | \
		LIBUSB_REQUEST_TYPE_CLASS | \
		LIBUSB_RECIPIENT_INTERFACE)

#define bmREQUEST_IN \
	(LIBUSB_ENDPOINT_IN | \
		LIBUSB_REQUEST_TYPE_CLASS | \
		LIBUSB_RECIPIENT_INTERFACE)

#define TIMEOUT 4000

static gboolean _should_have_dev;
static libusb_device_handle *_devh;

static void _build_cmds(unsigned char cmdv[CMDS_MAX][CMD_LEN])
{
	int i;

	for (i = 0; i < 3; i++) {
		memcpy(cmdv[i], layout_cmds[i], CMD_LEN);
	}

	memcpy(cmdv[3], light_level_cmd, CMD_LEN);
	memcpy(cmdv[4], pulsate_cmd, CMD_LEN);

	cmdv[0][ARG1I] = layout_vals[state.layout].a.a;
	cmdv[0][ARG2I] = layout_vals[state.layout].a.b;
	cmdv[1][ARG1I] = layout_vals[state.layout].b.a;
	cmdv[1][ARG2I] = layout_vals[state.layout].b.b;
	cmdv[2][ARG1I] = layout_vals[state.layout].c.a;
	cmdv[2][ARG2I] = layout_vals[state.layout].c.b;

	if (state.progi == -1) {
		cmdv[3][ARG1I] = light_levels[bright_off].a;
		cmdv[3][ARG2I] = light_levels[bright_off].b;

		cmdv[4][ARG1I] = pulsate_vals[FALSE].a;
		cmdv[4][ARG2I] = pulsate_vals[FALSE].b;
	} else {
		cmdv[3][ARG1I] = light_levels[cfg.usb.brightness].a;
		cmdv[3][ARG2I] = light_levels[cfg.usb.brightness].b;

		cmdv[4][ARG1I] = pulsate_vals[cfg.usb.pulse].a;
		cmdv[4][ARG2I] = pulsate_vals[cfg.usb.pulse].b;
	}
}

static void _sync(void)
{
	int i;
	int err;
	struct libusb_config_descriptor *dcfg;
	unsigned char cmdv[CMDS_MAX][CMD_LEN];
	int ok = 0;

	if (_devh == NULL) {
		return;
	}

	err = libusb_get_config_descriptor(libusb_get_device(_devh), 0, &dcfg);
	if (err != LIBUSB_SUCCESS) {
		usb_perror(err, "failed to fetch device config");
		goto out;
	}

	for (i = 0; i < dcfg->bNumInterfaces; i++) {
		int iface = dcfg->interface[i].altsetting->bInterfaceNumber;
		if (!libusb_kernel_driver_active(_devh, iface)) {
			continue;
		}

		err = libusb_detach_kernel_driver(_devh, iface);
		if (err != LIBUSB_SUCCESS) {
			usb_perror(err, "failed to detach kernel driver");
			goto out;
		}
	}

	err = libusb_set_configuration(_devh, dcfg->bConfigurationValue);
	if (err != LIBUSB_SUCCESS) {
		usb_perror(err, "failed to set device configuration");
		goto out;
	}

	err = libusb_claim_interface(_devh, wINDEX);
	if (err != LIBUSB_SUCCESS) {
		usb_perror(err, "failed to claim interface %d", wINDEX);
		goto out;
	}

	_build_cmds(cmdv);
	for (i = 0; i < CMDS_MAX; i++) {
		err = libusb_control_transfer(_devh,
			bmREQUEST_OUT,
			LIBUSB_REQUEST_SET_CONFIGURATION,
			wVALUE,
			wINDEX,
			cmdv[i], CMD_LEN,
			TIMEOUT);
		if (err < LIBUSB_SUCCESS) {
			usb_perror(err, "out control transfer failed %d", err);
			goto out;
		}

		// I'm not sure this is necessary, but the Windows util does it for
		// some reason
		err = libusb_control_transfer(_devh,
			bmREQUEST_IN,
			LIBUSB_REQUEST_CLEAR_FEATURE,
			wVALUE,
			wINDEX,
			cmdv[i], CMD_LEN,
			TIMEOUT);
		if (err < LIBUSB_SUCCESS) {
			usb_perror(err, "in control transfer failed");
			goto out;
		}
	}

	ok = 1;

out:
	libusb_release_interface(_devh, wINDEX);

	for (i = 0; i < dcfg->bNumInterfaces; i++) {
		libusb_attach_kernel_driver(_devh,
			dcfg->interface[i].altsetting->bInterfaceNumber);
	}

	libusb_free_config_descriptor(dcfg);

	if (!ok) {
		libusb_close(_devh);
		_devh = NULL;
	}
}

static void _poll_cb(int fd G_GNUC_UNUSED)
{
	libusb_handle_events_completed(NULL, NULL);
}

static void _fd_added(int fd, short events, void *nothing G_GNUC_UNUSED)
{
	poll_mod(fd, _poll_cb, events & POLLIN, events & POLLOUT);
}

static void _fd_removed(int fd, void *nothing G_GNUC_UNUSED)
{
	poll_rm(fd);
}

static int _hotplug(
	libusb_context *ctx G_GNUC_UNUSED,
	libusb_device *dev,
	libusb_hotplug_event event,
	void *user_data G_GNUC_UNUSED)
{
	int err;

	libusb_close(_devh);
	_devh = NULL;
	_should_have_dev = FALSE;

	if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
		g_debug("new usb device detected");

		_should_have_dev = TRUE;
		err = libusb_open(dev, &_devh);
		if (err != 0) {
			usb_perror(err, "failed to open USB device");
		} else {
			_sync();
		}
	} else {
		g_debug("usb device removed");
	}

	return 0;
}

void usb_init()
{
	int err;
	guint i;
	const struct libusb_pollfd **fds;

	err = libusb_init(NULL);
	if (err < 0) {
		usb_perror(err, "failed to init libusb");
		exit(2);
	}

	i = 0;
	fds = libusb_get_pollfds(NULL);
	while (fds[i] != NULL) {
		_fd_added(fds[i]->fd, fds[i]->events, NULL);
		i++;
	}

	free(fds);

	libusb_set_pollfd_notifiers(NULL, _fd_added, _fd_removed, NULL);
	libusb_hotplug_register_callback(NULL,
		LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
		LIBUSB_HOTPLUG_ENUMERATE,
		VENDOR, PRODUCT,
		LIBUSB_HOTPLUG_MATCH_ANY,
		_hotplug, NULL, NULL);
}

void usb_on_state_changed()
{
	_sync();
}

void usb_on_poll_tick()
{
	if (_should_have_dev && _devh == NULL) {
		_devh = libusb_open_device_with_vid_pid(NULL, VENDOR, PRODUCT);
	}
}

void usb_perror(int err, const char *format, ...)
{
	va_list args;
	GString *buff = g_string_new("");

	va_start(args, format);
	g_string_vprintf(buff, format, args);
	va_end(args);

	g_critical("%s: %s", buff->str, libusb_strerror(err));
	g_string_free(buff, TRUE);
}
