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

#include <libusb.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "const.h"
#include "usb.h"
#include "util.h"

#define bmREQUEST_OUT \
	(LIBUSB_ENDPOINT_OUT | \
		LIBUSB_REQUEST_TYPE_CLASS | \
		LIBUSB_RECIPIENT_INTERFACE)

#define bmREQUEST_IN \
	(LIBUSB_ENDPOINT_IN | \
		LIBUSB_REQUEST_TYPE_CLASS | \
		LIBUSB_RECIPIENT_INTERFACE)

#define TIMEOUT 2000

static int _layout;
static int _pulse;
static enum usb_brightness _brightness;
static libusb_device_handle *_devh;

void usb_init()
{
	int err;

	err = libusb_init(NULL);
	if (err < 0) {
		usb_perror(err, "failed to init libusb");
		exit(2);
	}
}

void usb_set_brightness(enum usb_brightness brightness)
{
	_brightness = brightness;
}

void usb_set_pulse(int pulse)
{
	_pulse = pulse;
}

int usb_connected()
{
	return _devh != NULL;
}

void usb_poll(void)
{
	if (usb_connected()) {
		return;
	}

 	_devh = libusb_open_device_with_vid_pid(NULL, VENDOR, PRODUCT);
 	if (_devh == NULL) {
 		// Still not connected...
 		return;
 	}

 	// Cool, the device connected! Sync settings to it
 	usb_commit();
}

static void _build_cmds(unsigned char cmdv[CMDS_MAX][CMD_LEN])
{
	int i;

	for (i = 0; i < 3; i++) {
		memcpy(cmdv[i], layout_cmds[i], CMD_LEN);
	}

	memcpy(cmdv[3], pulsate_cmd, CMD_LEN);
	memcpy(cmdv[4], light_level_cmd, CMD_LEN);

	cmdv[0][ARG1I] = layout_vals[_layout].a.a;
	cmdv[0][ARG2I] = layout_vals[_layout].a.b;
	cmdv[1][ARG1I] = layout_vals[_layout].b.a;
	cmdv[1][ARG2I] = layout_vals[_layout].b.b;
	cmdv[2][ARG1I] = layout_vals[_layout].c.a;
	cmdv[2][ARG2I] = layout_vals[_layout].c.b;

	cmdv[3][ARG1I] = pulsate_vals[_pulse].a;
	cmdv[3][ARG2I] = pulsate_vals[_pulse].b;

	cmdv[4][ARG1I] = light_levels[_brightness].a;
	cmdv[4][ARG2I] = light_levels[_brightness].b;
}

void usb_commit()
{
	int i;
	int err;
	struct libusb_config_descriptor *cfg;
	unsigned char cmdv[CMDS_MAX][CMD_LEN];
	int ok = 0;

	err = libusb_get_config_descriptor(libusb_get_device(_devh), 0, &cfg);
	if (err != LIBUSB_SUCCESS) {
		usb_perror(err, "failed to fetch device config");
		goto out;
	}

	for (i = 0; i < cfg->bNumInterfaces; i++) {
		int iface = cfg->interface[i].altsetting->bInterfaceNumber;
		if (!libusb_kernel_driver_active(_devh, iface)) {
			continue;
		}

		err = libusb_detach_kernel_driver(_devh, iface);
		if (err != LIBUSB_SUCCESS) {
			usb_perror(err, "failed to detach kernel driver");
			goto out;
		}
	}

	err = libusb_set_configuration(_devh, cfg->bConfigurationValue);
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

	for (i = 0; i < cfg->bNumInterfaces; i++) {
		libusb_attach_kernel_driver(_devh,
			cfg->interface[i].altsetting->bInterfaceNumber);
	}

	libusb_free_config_descriptor(cfg);

	if (!ok) {
		libusb_close(_devh);
		_devh = NULL;
	}
}

void usb_perror(int err, const char *format, ...)
{
	va_list args;
	char msg[2048];

	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	va_end(args);

	fprintf(stderr, "%s: %s\n", msg, libusb_strerror(err));
}
