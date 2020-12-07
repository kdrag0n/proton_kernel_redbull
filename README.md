# kramflash

kramflash is a simple bootable ramdisk that flashes a custom kernel on devices with boot image v3. It is not capable of flashing real GKIs because it doesn't manipulate any ramdisks for compatibility reasons, but it works with the GKI boot scheme and partition layout.

## Requirements

The flasher script, flash.sh, must be installed as the init script in an Alpine Linux ramdisk with the following changes made:

- `sgdisk` package installed
- `magiskboot` tool from Magisk placed in PATH
- `reboot_with_cmd` tool (source code available in this repo) placed in PATH

Additionally, the kernel must be built with `CONFIG_DEVTMPFS=y` for the script to work properly.

For the flasher to show output rather than appearing to freeze and crash at the bootloader splash, the kernel console needs to be configured to display output to the user. The easiest way to do this is to enable and use the default fbcon console and make it render to simplefb, backed by the continuous splash framebuffer that the bootloader sets up before starting Linux.

If a console is present, adding `loglevel=2` to the kernel command-line is recommended to reduce spammy output and speed up boot significantly.

## Payload

The kernel payload should be placed in the `/payload` directory of the ramdisk, consisting of:

- `Image.lz4`: LZ4-compressed kernel image **without** appended DTBs
- `dtb`: Concatenated DTBs for supported devices
- `banner`: Optional banner to show to the user when flashing

DTBO flashing is not currently supported.

## Kernel changes

The flashed kernel payload should not be changed in any way. However, the flasher itself uses a kernel because it is essentially a custom bootable recovery, and the kernel needs small changes at minimum to work in the flasher.

Mandatory changes:

- Enable `CONFIG_DEVTMPFS` ([example](https://github.com/kdrag0n/proton_kernel_redbull/commit/c227c0b7727ff358898f29058956eac00385bf0d))

Nice-to-have changes:

- For easier debugging on newer kernels (4.14+): add `printk.devkmsg=on` to cmdline ([example](https://github.com/kdrag0n/proton_kernel_redbull/commit/bc1bbca33cb2b08a267589847bf70ec56d64e542))
- Reduce kernel size to reduce the flasher size: [example](https://github.com/kdrag0n/proton_kernel_redbull/commit/6f5149c116cfa59eb8880090f9b894bb4a2d5d32)
- Framebuffer console for live feedback, otherwise it just looks like the flasher crashed even if it succeeded: [example framebuffer](https://github.com/kdrag0n/proton_kernel_redbull/commit/51f31de9dddf826e5be7eb69607111e6780c2d6c) and [console](https://github.com/kdrag0n/proton_kernel_redbull/commit/2887bed510ad89d33801e5f898fbf571ae808370)

If using a console:

- Silence kernel logs: add `quiet` to cmdline ([example](https://github.com/kdrag0n/proton_kernel_redbull/commit/9a1d781c435d2bbcbe274ae91eda81f621c3a93a)) and set loglevel ([example](https://github.com/kdrag0n/proton_kernel_redbull/commit/e9c11c12f9fee19c88fa8b81d931410879b028d6))
- Larger font for legibility: Terminus 16x32 ([example](https://github.com/kdrag0n/proton_kernel_redbull/commit/de9ada794065b2076739b22a4439f35d64d66171))

See the changes made to the [Pixel 5 kernel](https://github.com/kdrag0n/proton_kernel_redbull/commits/alpine) for more information.
