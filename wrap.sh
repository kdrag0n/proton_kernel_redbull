#!/bin/sh

# Populate PATH and other basic settings
source /etc/profile

# Mount pseudo-filesystems
mount -t devtmpfs devtmpfs /dev
mount -t proc proc /proc
mount -t sysfs sysfs /sys
#mount -t debugfs debugfs /sys/kernel/debug
mount -t configfs configfs /sys/kernel/config
mount -t tmpfs tmpfs /tmp
mkdir /dev/pts
mount -t devpts devpts /dev/pts

sh /wrc

sleep 1
reboot_with_cmd bootloader
