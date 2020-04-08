# SPDX-License-Identifier: GPL-2.0-only

# auto-detect subdirs
ifeq ($(CONFIG_SPECTRA_CAMERA:M=m), m)
SUFFIX_SPECTRA_CAMERA:=-gki
else
SUFFIX_SPECTRA_CAMERA:=
endif

ifeq ($(CONFIG_ARCH_KONA), y)
include $(srctree)/techpack/camera/config/konacamera$(SUFFIX_SPECTRA_CAMERA).conf
endif

ifeq ($(CONFIG_ARCH_LITO), y)
include $(srctree)/techpack/camera/config/litocamera$(SUFFIX_SPECTRA_CAMERA).conf
endif

ifeq ($(CONFIG_ARCH_BENGAL), y)
include $(srctree)/techpack/camera/config/bengalcamera.conf
endif

ifeq ($(CONFIG_ARCH_KONA), y)
LINUXINCLUDE    += \
		-include $(srctree)/techpack/camera/config/konacameraconf$(SUFFIX_SPECTRA_CAMERA).h
endif

ifeq ($(CONFIG_ARCH_LITO), y)
LINUXINCLUDE    += \
		-include $(srctree)/techpack/camera/config/litocameraconf$(SUFFIX_SPECTRA_CAMERA).h
endif

ifeq ($(CONFIG_ARCH_BENGAL), y)
LINUXINCLUDE    += \
		-include $(srctree)/techpack/camera/config/bengalcameraconf.h
endif

ifdef CONFIG_SPECTRA_CAMERA
# Use USERINCLUDE when you must reference the UAPI directories only.
USERINCLUDE     += \
                -I$(srctree)/techpack/camera/include/uapi

# Use LINUXINCLUDE when you must reference the include/ directory.
# Needed to be compatible with the O= option
LINUXINCLUDE    += \
                -I$(srctree)/techpack/camera/include/uapi \
                -I$(srctree)/techpack/camera/include
obj-y += drivers/
else
$(info Target not found)
endif
