# SPDX-License-Identifier: GPL-2.0-only

# auto-detect subdirs
ifeq ($(CONFIG_DRM_MSM:M=m), m)
SUFFIX_DRM_MSM:=-gki
else
SUFFIX_DRM_MSM:=
endif

ifeq ($(CONFIG_ARCH_KONA), y)
include $(srctree)/techpack/display/config/konadisp$(SUFFIX_DRM_MSM).conf
endif

ifeq ($(CONFIG_ARCH_KONA), y)
LINUXINCLUDE    += -include $(srctree)/techpack/display/config/konadispconf$(SUFFIX_DRM_MSM).h
endif

ifeq ($(CONFIG_ARCH_LITO), y)
include $(srctree)/techpack/display/config/saipdisp$(SUFFIX_DRM_MSM).conf
endif

ifeq ($(CONFIG_ARCH_LITO), y)
LINUXINCLUDE    += -include $(srctree)/techpack/display/config/saipdispconf$(SUFFIX_DRM_MSM).h
endif

ifeq ($(CONFIG_ARCH_BENGAL), y)
include $(srctree)/techpack/display/config/bengaldisp.conf
endif

ifeq ($(CONFIG_ARCH_BENGAL), y)
LINUXINCLUDE    += -include $(srctree)/techpack/display/config/bengaldispconf.h
endif

obj-$(CONFIG_DRM_MSM) += msm/
obj-$(CONFIG_MSM_SDE_ROTATOR) += rotator/
