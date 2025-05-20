# Desk_Control
This is a gtk GUI application to adjust volume and brightness. OS: Linux

The means of which volume and brightness are handled varies from machine to machine, distro to distro.

In my case, the volume is handled by amixer and the brightness is handled by intel_backlight.

See: https://wiki.archlinux.org/title/Backlight

Requires GTK4 development packages for compiling.

WARNING: brightness control modifies files found under the sys directory. Be advised.

Package Names
------------------------
Debian/Ubuntu: libgtk-4-dev

Fedora/SUSE: gtk4-devel

Arch: gtk4

run `make` to create binary.

For more info: https://www.gtk.org/docs/
