import usb.core
import usb.util

##
# USB Constants
##
USB_VENDOR = 0x1532
USB_PRODUCT = 0x0201
bREQ_TYPE_OUT = usb.util.build_request_type(
			usb.util.CTRL_OUT,
			usb.util.CTRL_TYPE_CLASS,
			usb.util.CTRL_RECIPIENT_INTERFACE)
bREQ_TYPE_IN = usb.util.build_request_type(
			usb.util.CTRL_IN,
			usb.util.CTRL_TYPE_CLASS,
			usb.util.CTRL_RECIPIENT_INTERFACE)
wVALUE = 0x0300
wINDEX = 2

##
# Command Constants
##

# Values to set in a light level request
LIGHT_LEVELS = {
	'h': (0xff, 0xf8), # high
	'm': (0xa3, 0xa4), # medium
	'l': (0x49, 0x4e), # low
	'o': (0x00, 0x07), # off
}

# Values to set in a pulsate request
PULSATE_VALS = [
	(0x00, 0x06), # solid
	(0x02, 0x04), # pulsate
]

LAYOUT_VALS = [
	((0x00, 0x0d), (0x00, 0x0c), (0x01, 0x0e)),
	((0x01, 0x0c), (0x00, 0x0c), (0x00, 0x0f)),
	((0x00, 0x0d), (0x01, 0x0d), (0x00, 0x0f)),
	((0x01, 0x0c), (0x00, 0x0c), (0x01, 0x0e)),
	((0x00, 0x0d), (0x01, 0x0d), (0x01, 0x0e)),
	((0x01, 0x0c), (0x01, 0x0d), (0x00, 0x0f)),
	((0x01, 0x0c), (0x01, 0x0d), (0x01, 0x0e)),
	((0x00, 0x0d), (0x00, 0x0c), (0x00, 0x0f)),
]

# Sent before and after every light level commmand.
# Only sent _before_ with pulsate commands.
LAYOUT_REQ = [
	'\x00\xff\x00\x00\x00\x03\x03\x00\x01\x0c%c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00%c\x00',
	'\x00\xff\x00\x00\x00\x03\x03\x00\x01\x0d%c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00%c\x00',
	'\x00\xff\x00\x00\x00\x03\x03\x00\x01\x0e%c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00%c\x00',
]

# Command to change light level
LIGHT_REQ = '\x00\xff\x00\x00\x00\x03\x03\x03\x01\x05%c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00%c\x00'

# Command to change pulsate flag
PULSATE_REQ = '\x00\xff\x00\x00\x00\x03\x03\x02\x01\x05%c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00%c\x00'

# Shared state between device and host
curr_layout = 1

def get_dev():
	dev = usb.core.find(idVendor=USB_VENDOR, idProduct=USB_PRODUCT)
	if not dev:
		raise ValueError('Device not found')
	return dev

def detach_kernel_driver(dev, interface_num):
	if dev.is_kernel_driver_active(interface_num):
		dev.detach_kernel_driver(interface_num)

def attach_kernel_driver(dev, interface_num):
	if not dev.is_kernel_driver_active(interface_num):
		try:
			usb.util.release_interface(dev, interface_num)
			dev.attach_kernel_driver(interface_num)
		except:
			pass

def map_intf(dev, fn):
	for cfg in dev:
		for intf in cfg:
			fn(dev, intf.bInterfaceNumber)

def write(cmds):
	dev = get_dev()

	try:
		map_intf(dev, detach_kernel_driver)

		dev.reset()
		dev.set_configuration()

		for cmd in cmds:
			dev.ctrl_transfer(
				bREQ_TYPE_OUT,
				usb.REQ_SET_CONFIGURATION,
				wVALUE, wINDEX, cmd)

			# I'm not sure this is necessary, but the Windows util does it for
			# some reason
			dev.ctrl_transfer(
				bREQ_TYPE_IN,
				usb.REQ_CLEAR_FEATURE,
				wVALUE, wINDEX, len(cmd))
	finally:
		map_intf(dev, attach_kernel_driver)

def get_layout():
	lv = LAYOUT_VALS[curr_layout - 1]
	return [
		LAYOUT_REQ[0] % lv[0],
		LAYOUT_REQ[1] % lv[1],
		LAYOUT_REQ[2] % lv[2],
	]

def set_light(level, with_trailer=True):
	layout = get_layout()
	msg =  LIGHT_REQ % LIGHT_LEVELS[level]
	msgs = layout + [msg]
	if with_trailer:
		msgs += layout
	write([msg])

def pulsate(on=True):
	layout = get_layout()
	write([PULSATE_REQ % PULSATE_VALS[on]])

def solid():
	pulsate(on=False)

def set_layout(i):
	global curr_layout
	if i < 1 or i > len(LAYOUT_VALS):
		raise ValueError("invalid layout: %d" % i)

	curr_layout = i
	write(get_layout())

def main():
	# set_layout(8)
	set_light('l')
	# solid()

if __name__ == '__main__':
	main()
