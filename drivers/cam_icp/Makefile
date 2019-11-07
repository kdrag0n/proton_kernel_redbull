# SPDX-License-Identifier: GPL-2.0-only

ccflags-y += -I$(srctree)/techpack/camera/include/uapi
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_utils
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_req_mgr
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_core
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_sync
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_icp
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_icp/icp_hw/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_icp/icp_hw/icp_hw_mgr
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_icp/fw_inc
ccflags-y += -I$(srctree)/techpack/camera
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cpas/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_smmu/

obj-$(CONFIG_SPECTRA_CAMERA) += icp_hw/
obj-$(CONFIG_SPECTRA_CAMERA) += cam_hfi.o
cam_hfi-objs += hfi.o
obj-$(CONFIG_SPECTRA_CAMERA) += cam_icp.o
cam_icp-objs += cam_icp_subdev.o cam_icp_context.o

ccflags-y += -I$(srctree)/techpack/camera/drivers/isp/isp_hw/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/isp/isp_hw/hw_utils/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/isp/isp_hw/isp_hw_mgr/
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_icp/icp_hw/icp_hw_mgr/include/
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_icp/icp_hw/a5_hw/
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cpas/include

cam_icp-objs += icp_hw/icp_hw_mgr/cam_icp_hw_mgr.o
