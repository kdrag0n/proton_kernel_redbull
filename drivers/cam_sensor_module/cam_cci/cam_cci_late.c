// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 */

#include <linux/module.h>

#include "cam_cci_dev.h"

static int __init __cam_cci_late_init(void)
{
	return cam_cci_late_init();
}

static void __exit __cam_cci_late_exit(void) { }

module_init(__cam_cci_late_init);
module_exit(__cam_cci_late_exit);
MODULE_DESCRIPTION("MSM CCI driver Late Initialization");
MODULE_LICENSE("GPL v2");
MODULE_SOFTDEP("pre: cam_req_mgr");
MODULE_SOFTDEP("pre: cam-sync");
MODULE_SOFTDEP("pre: cam_smmu_api");
MODULE_SOFTDEP("pre: cam_cpas");
MODULE_SOFTDEP("pre: cam_cdm");
MODULE_SOFTDEP("pre: cam_ife_csid17x");
MODULE_SOFTDEP("pre: cam_ife_csid_lite17x");
MODULE_SOFTDEP("pre: cam_vfe");
MODULE_SOFTDEP("pre: cam_isp");
MODULE_SOFTDEP("pre: cam_res_mgr");
MODULE_SOFTDEP("pre: cam_csiphy");
MODULE_SOFTDEP("pre: cam_actuator");
MODULE_SOFTDEP("pre: cam_sensor");
MODULE_SOFTDEP("pre: cam_eeprom");
MODULE_SOFTDEP("pre: cam_ois");
MODULE_SOFTDEP("pre: cam_flash");
MODULE_SOFTDEP("pre: cam_a5");
MODULE_SOFTDEP("pre: cam_ipe");
MODULE_SOFTDEP("pre: cam_bps");
MODULE_SOFTDEP("pre: cam_icp");
MODULE_SOFTDEP("pre: cam_jpeg_enc");
MODULE_SOFTDEP("pre: cam_jpeg_dma");
MODULE_SOFTDEP("pre: cam_jpeg");
MODULE_SOFTDEP("pre: cam-fd-hw-mgr");
MODULE_SOFTDEP("pre: cam_fd");
MODULE_SOFTDEP("pre: cam_lrme_hw");
MODULE_SOFTDEP("pre: cam_lrme");
MODULE_SOFTDEP("pre: cam_req_mgr_late");
