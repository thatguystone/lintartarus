# lintartarus

Key mapping and light control for the Razer Tartarus on Linux

## Keys

The Tartarus conists of 21 keys: 15 keys on the key pad, 2 thumb buttons, and a 4-way directional pad. Each key or button is mapping to a character on a typical keyboard, and the OS registers the entire device as a big, ol' keyboard.

## Building

Dependencies:

* libusb-1.0-0-dev >= 1.0.16
* libglib2.0-dev >= 2.32

```bash
make
./lintartarus -h
```

If there's enough demand, I'll get some Debian packages setup.

## Setup

lintartarus performs key remapping by capturing the output of the entire device, remapping key sequences internally, and feeding the remapped key sequences back to the OS. To do this, you'll need to give yourself access to the necessary devices.

To authorize the plugdev group, run:
```bash
sudo ./lintartarus --authorize
```

To authorize a different group, run:
```bash
sudo ./lintartarus --authorize <group name>
```

And that's it. You might have to unplug your Tartarus and reconnect it for the changes to be applied. If you get permission errors with the uinput device, either change its group so you have access, or, if you don't know what the means, reboot. Since uinput already exists, udev typically won't change its groups when the udev config is updated.

## Config

Config files are, by default, placed in ~/.config/lintartarus. They are monitored for changes, and all changes will be reflected immediately.

There is a single setting for the device: its backlight. The backlight is only activated when a registered program is seen to be running, otherwise, all lights remain off.

The backlight may be configured with the following values: `off`, `low`, `med`, `high`, `pulse`.

## Key Maps

The keymaps I use can be found in the `keymaps` directory above.

This is the section you probably care about the most: how do you remap the f'ing keys to something useful in a game? Well, take a gander below:

```ini
[ksp]
# Steam path for KSP
# cmd = some command to look for
exe = Kerbal Space Program/KSP

# VAB controls
[ksp:1]
# Disable the first key
1 =

# Switch layouts with these keys
14 = LAYOUT_NEXT
15 = LAYOUT_NEXT

# KSP's mod key on Linux
thumb_up = Shift_R

# Flight controls
[ksp:2]
14 = LAYOUT_NEXT
15 = LAYOUT_NEXT
```

With this config, whenever lintartarus sees an executable with "Kerbal Space Program/KSP" in the path, it will activate the first layout for that program.

You might also care about arguments to a command; in that case, the `cmd` option will search for the given substring in the command used to invoke the program. Otherwise, use exe to look at command paths. If `exe` starts with a `/`, it will look at the entire path; otherwise, it will look for a substring of the path.

Layouts are numbered starting at 1, going up to 7. Any layout changes are reflected on the device's lights.

### Key Names

The keys have the following names:

1. `1-15`: keys on the key pad, as numbered
1. `up/down/left/right`: keys on the directional pad
1. `thumb_up`: top thumb button
1. `thumb_down`: bottom thumb button

### Key Combos

The key combos work like you would expect, except that there's no distinction between upper and lower case. You have to add a shift to the combo if you want to use uppercase. That also means that, in the configuration files, key names are not case sensitive.

Examples:

1. `ctrl+l`: trigger l while ctrl is being held
1. `ctrl+shift+l`: trigger capital L while ctrl is being held
1. `ctrl+l ctrl+a`: first trigger ctrl+l, release, then trigger ctrl+a, then release

There are two special key names `LAYOUT_NEXT` and `LAYOUT_PREV`. Assign these to any key to allow you cycle through different layouts while in game. Typically, you'll only have a single layout for a game, but for some complicated games, multiple layouts is handy.

If you want to disable a key, simply set its values to a blank line.
