#!/bin/sh

set -eufo pipefail

BLOCK_DEV=/dev/sda
PAYLOAD_DIR=/payload

# Populate PATH and other basic settings
source /etc/profile

reboot_end() {
    echo "Rebooting in 3 seconds..."
    # Rounded corner protection
    echo
    echo
    sleep 3

    # Wait for volume down keypress
    #read -n1
    # Wait for manual forced reboot
    #sleep inf

    # Busybox reboot doesn't work for some reason
    reboot_with_cmd "$1"
}

on_exit() {
    echo
    echo
    echo "ERROR!"

    reboot_end bootloader
}

# Set trap before mounting in case devtmpfs fails
trap on_exit EXIT

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






$(cat $PAYLOAD_DIR/banner 2>/dev/null || echo -e 'Installing kernel with kramflash by kdrag0n')

---------------------------------

EOF

boot_slot="$(cat /proc/cmdline | tr ' ' '\n' | grep androidboot.slot_suffix | sed 's/.*=_\(.*\)/\1/')"
echo "Current slot: $boot_slot"
echo

boot_part="$(find_part_by_name boot_$boot_slot)"
vendor_boot_part="$(find_part_by_name vendor_boot_$boot_slot)"

echo "Unpacking boot images"
mkdir -p /tmp/boot /tmp/vendor_boot
cd /tmp/boot
boot_info="$(magiskboot unpack -n "$boot_part" 2>&1)"
boot_ver="$(echo "$boot_info" | grep HEADER_VER | awk '{print $2}' | tr -d '[]')"
echo "  • Boot image version: v$boot_ver"
cd ../vendor_boot
vendor_boot_info="$(magiskboot unpack -n "$vendor_boot_part" 2>&1)"

echo "Modifying boot images"
cd ..
cp "$PAYLOAD_DIR/Image.lz4" boot/kernel
cp "$PAYLOAD_DIR/dtb" vendor_boot/dtb
# For v2
cp "$PAYLOAD_DIR/dtb" boot/dtb

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

reboot_end ''
