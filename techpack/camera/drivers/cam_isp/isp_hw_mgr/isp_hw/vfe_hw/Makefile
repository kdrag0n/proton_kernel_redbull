# SPDX-License-Identifier: GPL-2.0-only

ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_utils
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_core
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cpas/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/isp_hw/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/hw_utils/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/hw_utils/irq_controller
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/isp_hw/vfe_hw/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/isp_hw/vfe_hw/vfe_bus/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/isp_hw/vfe_hw/vfe_top/include
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_req_mgr

obj-$(CONFIG_SPECTRA_CAMERA) += cam_vfe.o
cam_vfe-objs += cam_vfe_soc.o cam_vfe_dev.o cam_vfe_core.o

ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_cdm/
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/isp_hw/vfe_hw/

# vfe_bus
cam_vfe-objs += vfe_bus/cam_vfe_bus.o vfe_bus/cam_vfe_bus_ver2.o
cam_vfe-objs += vfe_bus/cam_vfe_bus_rd_ver1.o vfe_bus/cam_vfe_bus_ver3.o

# vfe_top
cam_vfe-objs += vfe_top/cam_vfe_camif_lite_ver2.o vfe_top/cam_vfe_top.o
cam_vfe-objs += vfe_top/cam_vfe_top_common.o vfe_top/cam_vfe_top_ver3.o
cam_vfe-objs += vfe_top/cam_vfe_top_ver2.o vfe_top/cam_vfe_camif_ver2.o
cam_vfe-objs += vfe_top/cam_vfe_camif_ver3.o vfe_top/cam_vfe_rdi.o
cam_vfe-objs += vfe_top/cam_vfe_fe_ver1.o vfe_top/cam_vfe_camif_lite_ver3.o

ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/isp_hw/vfe_hw/vfe_top
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/isp_hw/vfe_hw/vfe_bus
ccflags-y += -I$(srctree)/techpack/camera/drivers/cam_isp/isp_hw_mgr/isp_hw/vfe_hw

cam_vfe-objs += vfe17x/cam_vfe.o
