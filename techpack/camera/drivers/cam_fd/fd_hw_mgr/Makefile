# SPDX-License-Identifier: GPL-2.0-only

ccflags-y += -I$(srctree)/techpack/camera/include/uapi
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_utils
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_req_mgr
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_core
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_sync
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_smmu
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cdm
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_fd
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_fd/fd_hw_mgr
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_fd/fd_hw_mgr/fd_hw
ccflags-y += -I$(srctree)/techpack/camera/drivers/
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cpas/include

obj-$(CONFIG_SPECTRA_CAMERA) += cam-fd-hw-mgr.o
cam-fd-hw-mgr-objs += cam_fd_hw_mgr.o

ccflags-y += -I$(srctree)/techpack/camera

cam-fd-hw-mgr-objs += fd_hw/cam_fd_hw_dev.o fd_hw/cam_fd_hw_core.o
cam-fd-hw-mgr-objs += fd_hw/cam_fd_hw_soc.o
