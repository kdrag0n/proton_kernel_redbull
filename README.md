# kramflash

kramflash is a simple bootable ramdisk that flashes a custom kernel on devices with boot image v3. It is not capable of flashing real GKIs because it doesn't manipulate any ramdisks for compatibility reasons, but it works with the 

## Requirements

The flasher script, flash.sh, must be installed as the init script in an Alpine Linux ramdisk with the following changes made:

- `sgdisk` package installed
- `magiskboot` tool from Magisk placed in PATH
- `reboot_with_cmd` tool (source code available in this repo) placed in PATH

Additionally, the kernel must be built with `CONFIG_DEVTMPFS=y` for the script to work properly.

For the flasher to show output rather than appearing to freeze and crash at the bootloader splash, the kernel console needs to be configured to display output to the user. The easiest way to do this is to enable and use the default fbcon console and make it render to simplefb, backed by the continuous splash framebuffer that the bootloader sets up before starting Linux.

If a console is present, adding `loglevel=2` to the kernel command-line is recommended to reduce spammy output and speed up boot significantly.
