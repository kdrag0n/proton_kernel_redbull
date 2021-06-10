/* SPDX-License-Identifier: GPL-2.0-only */
/* OIS calibration interface for LC898129dp
 *
 */
#include "../cam_fw_update/PhoneUpdate.h"
#include "../cam_sensor/cam_sensor_dev.h"

void RamWrite32A(UINT_16 RamAddr, UINT_32 RamData);
void RamRead32A(UINT_16 RamAddr, UINT_32 *ReadData);
void WitTim(UINT_16);
bool checkOISFWversion(UINT_16 *cal_id, UINT_32 *module_maker);
int checkOISFWUpdate(struct cam_sensor_ctrl_t *s_ctrl);
int CntRd(UINT_32 addr, void *PcSetDat, UINT_16 UsDatNum);
int CntWrt(UINT_8 *PcSetDat, UINT_16 CntWrt);
int GyroReCalibrate(struct camera_io_master *io_master_info,
	stReCalib *cal_result);
int WrGyroOffsetData(struct camera_io_master *io_master_info,
	stReCalib *cal_result);
int getFWVersion(struct cam_sensor_ctrl_t *s_ctrl);
int GyroOffsetCorrect(struct camera_io_master *io_master_info,
	uint32_t gyro_correct_index);
