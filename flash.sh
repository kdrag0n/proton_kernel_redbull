#!/bin/sh

# REQUIREMENTS
# Packages: sgdisk lz4
# Tools: /usr/bin/magiskboot /usr/bin/reboot_with_cmd
# Kernel config: CONFIG_DEVTMPFS
# Kernel cmdline: printk.devkmsg=on

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
mkdir -p /tmp/new /tmp/boot /tmp/vendor_boot
cd /tmp/new
magiskboot unpack -nh "$PAYLOAD_DIR/boot_v2.img" 2>/dev/null

cd ../boot
src_boot_info="$(magiskboot unpack -nh "$boot_part" 2>&1)"
src_boot_ver="$(echo "$src_boot_info" | grep '^HEADER_VER' | awk '{print $2}' | tr -d '[]')"

cd ../vendor_boot
magiskboot unpack -nh "$vendor_boot_part" 2>/dev/null
vendor_cmdline="$(grep '^cmdline=' header)"

echo "Modifying boot image"
cd ../new
echo "  - Metadata"
cp ../boot/header .
sed -i "s|cmdline=$|cmdline=$vendor_cmdline|" header
# Treatment for initial transition from v3
if [ "$src_boot_ver" -eq 3 ]; then
    echo "  - Ramdisk"
    lz4 -dc ../boot/ramdisk.cpio > raw_ramdisk.cpio
    lz4 -dc ../vendor_boot/ramdisk.cpio >> raw_ramdisk.cpio
    lz4 -lc raw_ramdisk.cpio > ramdisk.cpio
fi
echo "  - Kernel"
# currently a no-op

echo "Repacking boot image"
magiskboot repack -n "$PAYLOAD_DIR/boot_v2.img" new.img 2>/dev/null

echo "Flashing new boot image"
cat new.img > "$boot_part"
sync

echo
echo "Kernel installed!"
echo "Rebooting in 3 seconds..."
sleep 3
# Busybox reboot doesn't work for some reason
reboot_with_cmd ''
