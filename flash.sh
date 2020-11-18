#!/bin/sh

# REQUIREMENTS
# Packages: sgdisk
# Tools: /usr/bin/magiskboot /usr/bin/reboot_with_cmd
# Kernel config: CONFIG_DEVTMPFS

# OPTIONAL
# Kernel cmdline: printk.devkmsg=on
# Kernel console: fbcon on simplefb using bootloader framebuffer

set -eufo pipefail

BLOCK_DEV=/dev/sda
PAYLOAD_DIR=/payload

# Populate PATH and other basic settings
source /etc/profile

# Mount essential pseudo-filesystems
mount -t devtmpfs devtmpfs /dev
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t tmpfs tmpfs /tmp

find_part_by_name() {
    partnum="$(sgdisk -p "$BLOCK_DEV" | grep " $1$" | head -n1 | awk '{print $1}')"
    echo "$BLOCK_DEV$partnum"
}

# Blank lines are for rounded corner & camera cutout protection
cat <<EOF






 ____            _              
|  _ \ _ __ ___ | |_ ___  _ __  
| |_) | '__/ _ \| __/ _ \| '_ \ 
|  __/| | | (_) | || (_) | | | |
|_|   |_|  \___/ \__\___/|_| |_|

Proton Kernel for Pixel 5 & 4a 5G
        v1.0 • by kdrag0n

----------------------------------------

EOF

boot_slot="$(cat /proc/cmdline | tr ' ' '\n' | grep androidboot.slot_suffix | sed 's/.*=_\(.*\)/\1/')"
echo "Current slot: $boot_slot"
echo

boot_part="$(find_part_by_name boot_$boot_slot)"
vendor_boot_part="$(find_part_by_name vendor_boot_$boot_slot)"

echo "Unpacking boot images"
mkdir -p /tmp/boot /tmp/vendor_boot
cd /tmp/boot
magiskboot unpack -n "$boot_part" 2>/dev/null
cd ../vendor_boot
magiskboot unpack -n "$vendor_boot_part" 2>/dev/null

echo "Modifying boot images"
cd ..
cp "$PAYLOAD_DIR/Image.lz4" boot/kernel
cp "$PAYLOAD_DIR/dtb" vendor_boot/dtb
echo "Repacking boot images"
cd boot
magiskboot repack -n "$boot_part" new.img 2>/dev/null
cd ../vendor_boot
magiskboot repack -n "$vendor_boot_part" new.img 2>/dev/null

echo "Flashing new boot images"
cd ..
cat boot/new.img > "$boot_part"
cat vendor_boot/new.img > "$vendor_boot_part"
sync

echo
echo "Kernel installed!"
echo "Rebooting in 2 seconds..."
sleep 2
# Busybox reboot doesn't work for some reason
reboot_with_cmd ''
