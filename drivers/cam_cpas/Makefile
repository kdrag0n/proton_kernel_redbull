# SPDX-License-Identifier: GPL-2.0-only

ccflags-y += -I$(srctree)/techpack/camera/include/uapi
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_req_mgr
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_core
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cpas/include
#ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cpas/cpas_top
#ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cpas/camss_top
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_utils
ccflags-y += -I$(srctree)

obj-$(CONFIG_SPECTRA_CAMERA) += cam_cpas.o
cam_cpas-objs += cam_cpas_soc.o cam_cpas_intf.o cam_cpas_hw.o

ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cpas

cam_cpas-objs += cpas_top/cam_cpastop_hw.o

cam_cpas-objs += camss_top/cam_camsstop_hw.o
