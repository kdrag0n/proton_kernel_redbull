# SPDX-License-Identifier: GPL-2.0-only

# auto-detect subdirs
ifeq ($(CONFIG_MSM_VIDC_V4L2:M=m), m)
SUFFIX_MSM_VIDC_V4L2:=-gki
else
SUFFIX_MSM_VIDC_V4L2:=
endif

ifeq ($(CONFIG_ARCH_KONA), y)
include $(srctree)/techpack/video/config/konavid$(SUFFIX_MSM_VIDC_V4L2).conf
endif

ifeq ($(CONFIG_ARCH_KONA), y)
LINUXINCLUDE    += -include $(srctree)/techpack/video/config/konavidconf$(SUFFIX_MSM_VIDC_V4L2).h
endif

# auto-detect subdirs
ifeq ($(CONFIG_ARCH_LITO), y)
include $(srctree)/techpack/video/config/litovid$(SUFFIX_MSM_VIDC_V4L2).conf
endif

ifeq ($(CONFIG_ARCH_LITO), y)
LINUXINCLUDE    += -include $(srctree)/techpack/video/config/litovidconf$(SUFFIX_MSM_VIDC_V4L2).h
endif

# auto-detect subdirs
ifeq ($(CONFIG_ARCH_BENGAL), y)
include $(srctree)/techpack/video/config/bengalvid.conf
endif

ifeq ($(CONFIG_ARCH_BENGAL), y)
LINUXINCLUDE    += -include $(srctree)/techpack/video/config/bengalvidconf.h
endif

obj-y +=msm/
