// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
 */

/* Instantiate tracepoints */
#define CREATE_TRACE_POINTS
#include "cam_trace.h"

EXPORT_TRACEPOINT_SYMBOL_GPL(cam_apply_req);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_buf_done);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_cdm_cb);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_context_state);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_flush_req);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_icp_fw_dbg);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_irq_activated);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_irq_handled);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_isp_activated_irq);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_log_event);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_req_mgr_add_req);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_req_mgr_apply_request);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_req_mgr_connect_device);
EXPORT_TRACEPOINT_SYMBOL_GPL(cam_submit_to_hw);
