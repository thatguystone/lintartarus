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

#include "const.h"
#include "usb.h"

static int _inited = 0;

__attribute__((__noreturn__))
__attribute__((__format__(__printf__, 1,2)))
static void _error(const char *format, ...)
{
	va_list args;
	char msg[2304];

	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);

	fprintf(stderr, "%s\n", msg);
	exit(1);
}

__attribute__((__noreturn__))
__attribute__((__format__(__printf__, 2, 3)))
static void _perror(int err, const char *format, ...)
{
	va_list args;
	char msg[2048];

	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	_error("%s: %s", msg, libusb_strerror(err));
}

static void _init(void)
{
	int err;

	if (_inited) {
		return;
	}

	err = libusb_init(NULL);
	if (err < 0) {
		_perror(err, "failed to init libusb");
	}

	_inited = 1;
}

static libusb_device_handle* _get_devh(void)
{
	libusb_device_handle *devh;

	_init();

	devh = libusb_open_device_with_vid_pid(NULL, VENDOR, PRODUCT);
	if (devh == NULL) {
		_error("device not found; do you have permission?");
	}

	return devh;
}

static void _free_devh(libusb_device_handle *dev)
{
	libusb_close(dev);
	libusb_exit(NULL);
	_inited = 0;
}

static const char* _desc_type(int bDescriptorType)
{
	switch (bDescriptorType) {
		case LIBUSB_DT_DEVICE: return "device";
		case LIBUSB_DT_CONFIG: return "config";
		case LIBUSB_DT_STRING: return "string";
		case LIBUSB_DT_INTERFACE: return "interface";
		case LIBUSB_DT_ENDPOINT: return "endpoint";
		case LIBUSB_DT_BOS: return "bos";
		case LIBUSB_DT_DEVICE_CAPABILITY: return "device_capability";
		case LIBUSB_DT_HID: return "hid";
		case LIBUSB_DT_REPORT: return "report";
		case LIBUSB_DT_PHYSICAL: return "physical";
		case LIBUSB_DT_HUB: return "hub";
		case LIBUSB_DT_SUPERSPEED_HUB: return "superspeed_hub";
		case LIBUSB_DT_SS_ENDPOINT_COMPANION: return "ss_endpoint_companion";
		default: return "unknown";
	}
}

static const char* _ep_type(int bmAttributes)
{
	switch (bmAttributes & USB_EP_TYPE_MASK) {
		case LIBUSB_TRANSFER_TYPE_CONTROL: return "control";
		case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS: return "isochronous";
		case LIBUSB_TRANSFER_TYPE_BULK: return "bulk";
		case LIBUSB_TRANSFER_TYPE_INTERRUPT: return "interrupt";
		case LIBUSB_TRANSFER_TYPE_BULK_STREAM: return "stream";
		default: return "unknown";
	}
}

void usb_dump()
{
	int i;
	int j;
	int k;
	int err;
	struct libusb_config_descriptor *cfg;
	libusb_device_handle *devh = _get_devh();
	libusb_device *dev = libusb_get_device(devh);

	err = libusb_get_config_descriptor(dev, 0, &cfg);
	if (err < 0) {
		_perror(err, "failed to get device config");
	}

	printf("#interfaces = %d\n", cfg->bNumInterfaces);

	for (i = 0; i < cfg->bNumInterfaces; i++) {
		const struct libusb_interface *iface = &cfg->interface[i];
		printf("%d - altsettings = %d\n", i, iface->num_altsetting);

		for (j = 0; j < iface->num_altsetting; j++) {
			const struct libusb_interface_descriptor *id = &iface->altsetting[j];

			printf("\t\tdesctype   = %s\n", _desc_type(id->bDescriptorType));
			printf("\t\tclass      = %d\n", id->bInterfaceClass);
			printf("\t\tstr idx    = %d\n", id->iInterface);
			printf("\t\t#endpoints = %d\n", id->bNumEndpoints);

			for (k = 0; k < id->bNumEndpoints; k++) {
				const struct libusb_endpoint_descriptor *ep = &id->endpoint[k];

				printf("\t\t\taddr      = %d\n", ep->bEndpointAddress & USB_EP_ADDR_MASK);
				printf("\t\t\ttype      = %s\n", _desc_type(ep->bDescriptorType));
				printf("\t\t\tdirection = %s\n",
					(ep->bEndpointAddress & USB_EP_DIR_MASK) == LIBUSB_ENDPOINT_IN ? "in" : "out");
				printf("\t\t\ttype      = %s\n", _ep_type(ep->bmAttributes));
				printf("\n");
			}
		}
	}

	libusb_free_config_descriptor(cfg);
	_free_devh(devh);
}
