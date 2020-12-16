// SPDX-License-Identifier: GPL-2.0-only
/* OIS calibration interface for LC898129dp
 *
 */


#include "fw_update.h"

#define OIS_CUR_FW_VERSION           0x0D
#define OIS_REARWIDE_I2C_ADDR_WRITE  0x76
#define MV_LGIT                      0x09170000
#define MA_WIDE                      0x00000400

static struct camera_io_master *g_io_master_info;

void RamWrite32A(UINT_16 RamAddr, UINT_32 RamData)
{
	int rc = 0;
	struct camera_io_master *io_master_info = g_io_master_info;
	struct cam_sensor_i2c_reg_setting i2c_reg_settings;
	struct cam_sensor_i2c_reg_array i2c_reg_array;

	i2c_reg_settings.addr_type = CAMERA_SENSOR_I2C_TYPE_WORD;
	i2c_reg_settings.data_type = CAMERA_SENSOR_I2C_TYPE_DWORD;
	i2c_reg_settings.size = 1;
	i2c_reg_settings.delay = 0;
	i2c_reg_array.reg_addr = RamAddr;
	i2c_reg_array.reg_data = RamData;
	i2c_reg_array.delay = 0;
	i2c_reg_settings.reg_setting = &i2c_reg_array;

	rc = camera_io_dev_write(io_master_info, &i2c_reg_settings);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "[OISFW] %s : write i2c failed, sid:0x%x\n",
			__func__, io_master_info->cci_client->sid);
}

void RamRead32A(UINT_16 RamAddr, UINT_32 *ReadData)
{
	int rc = 0;

	rc = camera_io_dev_read(g_io_master_info, RamAddr,
		ReadData, CAMERA_SENSOR_I2C_TYPE_WORD,
		CAMERA_SENSOR_I2C_TYPE_DWORD);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "[OISFW]:%s read i2c failed, sid:0x%x\n",
			__func__, g_io_master_info->cci_client->sid);
}

void WitTim(UINT_16 UsWitTim)
{
	msleep(UsWitTim);
}

int CntWrt(UINT_8 *PcSetDat, UINT_16 UsDatNum)
{
	int rc = 0, cnt;
	uint16_t total_bytes = UsDatNum-1;
	uint8_t *ptr = NULL;
	struct camera_io_master *io_master_info = g_io_master_info;
	struct cam_sensor_i2c_reg_setting i2c_reg_setting;

	i2c_reg_setting.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_setting.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE;
	i2c_reg_setting.size = total_bytes;
	i2c_reg_setting.delay = 0;
	i2c_reg_setting.reg_setting = (struct cam_sensor_i2c_reg_array *)
		kzalloc(sizeof(struct cam_sensor_i2c_reg_array) * total_bytes,
		GFP_KERNEL);
	if (!i2c_reg_setting.reg_setting) {
		CAM_ERR(CAM_SENSOR, "[OISFW]:%s Failed in allocating i2c_array",
			__func__);
		return -ENOMEM;
	}

	for (cnt = 0, ptr = &PcSetDat[1]; cnt < total_bytes;
		cnt++, ptr++) {
		i2c_reg_setting.reg_setting[cnt].reg_addr = PcSetDat[0];
		i2c_reg_setting.reg_setting[cnt].reg_data = *ptr;
		i2c_reg_setting.reg_setting[cnt].delay = 0;
		i2c_reg_setting.reg_setting[cnt].data_mask = 0;
	}

	rc = camera_io_dev_write_continuous(
		io_master_info, &i2c_reg_setting, 1);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "[OISFW]:%s i2c write sequence error:%d\n",
			__func__, rc);

	kfree(i2c_reg_setting.reg_setting);
	return rc;
}

int CntRd(UINT_32 addr, void *PcSetDat, UINT_16 UsDatNum)
{
	int rc = 0;

	rc = camera_io_dev_read_seq(g_io_master_info, addr, PcSetDat,
		CAMERA_SENSOR_I2C_TYPE_WORD, CAMERA_SENSOR_I2C_TYPE_WORD,
		UsDatNum);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "[OISFW]:%s read i2c failed\n", __func__);
	return rc;
}

void WPBCtrl(UINT_8 UcCtrl)
{
	//do nothing because lc898129dp uses UnlockCodeSet()
	//to handle WPB by itself
}

int checkHighLevelCommand(int cnt)
{
	int i;
	UINT_32 FWRead;

	for (i = 0; i < cnt ; i++) {
		WitTim(10);
		RamRead32A(0xF100, &FWRead);//Check high level command ready.
		if (FWRead == 0x0) {
			CAM_INFO(CAM_SENSOR, "[OISFW]:%s finish.", __func__);
			return 0;
		}
		CAM_INFO(CAM_SENSOR, "[OISFW]:%s waiting...", __func__);
	}
	CAM_ERR(CAM_SENSOR, "[OISFW]:%s fail.", __func__);
	return -EINVAL;
}

int doFWupdate(UINT_16 CAL_ID, UINT_32 MODULE_MAKER)
{
	int rc = 0;
	UINT_8 code_vendor = 0;
	UINT_8 code_header = 0;
	UINT_32 module_vendor = MODULE_MAKER & 0xFFFF0000;
	UINT_32 module_angle = MODULE_MAKER & 0x0000FF00;

	if (module_vendor == MV_LGIT)
		code_vendor = 9;

	if (module_angle == MA_WIDE && CAL_ID == 0x3)
		code_header = 0x01;

	// If fw update failure, use recovery mechanism to enforce fw update.
	if(CAL_ID == 0x0 && module_vendor == 0x0) {
		code_vendor = 9;
		code_header = 0x01;
	}

	if (code_vendor != 0 && code_header != 0) {
		BootMode();
		CAM_INFO(CAM_SENSOR,
			"[OISFW]: BootMode(sid:0x%x), " \
			"start flash download(0x%x, 0x%x)\n",
			g_io_master_info->cci_client->sid,
			code_vendor, code_header);
		rc = FlashProgram129(0, code_vendor, code_header);
		WitTim(50);
	} else {
		CAM_ERR(CAM_SENSOR,
			"[OISFW]:%s unknown module_maker(0x%x) " \
			"or code_header(0x%x).",
			__func__, module_vendor, CAL_ID);
		rc = -EINVAL;
	}

	if (rc == 0) {
		/*Wait for FW update finish.*/
		rc = checkHighLevelCommand(20);
	} else {
		CAM_ERR(CAM_SENSOR,
			"[OISFW]%s: OIS FW update failed rc = %d.\n",
			__func__, rc);
	}

	return rc;
}

bool checkOISFWversion(UINT_16 *cal_id, UINT_32 *module_maker)
{
	int rc;
	UINT_16 RamAddr;
	UINT_32 UlReadVal;
	UINT_16 FW_version;
	bool need_update = false;

	CAM_INFO(CAM_SENSOR, "[OISFW]:%s\n", __func__);

	RamAddr = 0x8000;
	RamRead32A(RamAddr, &UlReadVal);
	*module_maker = UlReadVal;
	FW_version = UlReadVal & 0xFF;
	CAM_INFO(CAM_SENSOR, "[OISFW]:%s module_version =  0x%02x.\n",
		__func__, FW_version);

	RamAddr = 0x8004;
	RamRead32A(RamAddr, &UlReadVal);
	*cal_id = UlReadVal & 0xFF;
	CAM_INFO(CAM_SENSOR,
		"[OISFW]:%s CAL_ID = 0x%04x, MODULE_MAKER = 0x%x\n",
		__func__, *cal_id, *module_maker);

	if (FW_version >= OIS_CUR_FW_VERSION && FW_version != 0x00) {
		CAM_INFO(CAM_SENSOR,
			"[OISFW]%s: No need to update.\n", __func__);
	} else {
		rc = checkHighLevelCommand(100);
		if (rc != 0) {
			CAM_ERR(CAM_SENSOR,
				"[OISFW]:%s checkHighLevelCommand failed = %d\n",
				__func__, rc);
		} else
			need_update = true;
	}

	return need_update;
}

int checkOISFWUpdate(struct cam_sensor_ctrl_t *s_ctrl)
{
	int rc = 0;
	int i;
	unsigned short cci_client_sid_backup;
	UINT_32 FWRead;
	UINT_16 FW_version;
	UINT_16 cal_id;
	UINT_32 module_maker;

	if (s_ctrl->sensordata->slave_info.sensor_id != 0x363) {
		CAM_INFO(CAM_SENSOR,
			"[OISFW]%s: SensorId:0x%x no need update.\n",
			__func__, s_ctrl->sensordata->slave_info.sensor_id);
		return 0;
	}

	/* Bcakup the I2C slave address */
	cci_client_sid_backup = s_ctrl->io_master_info.cci_client->sid;

	/* Replace the I2C slave address with OIS component */
	if (s_ctrl->sensordata->slave_info.sensor_id == 0x363) {
		s_ctrl->io_master_info.cci_client->sid =
			OIS_REARWIDE_I2C_ADDR_WRITE >> 1;
	}

	g_io_master_info = &(s_ctrl->io_master_info);
	WitTim(100);

	/*Check current HW and FW version*/
	if (checkOISFWversion(&cal_id, &module_maker) == true) {
		rc = doFWupdate(cal_id, module_maker);

		for (i = 0; i < 2 ; i++) {
			RamRead32A(0x8000, &FWRead);
			FW_version = FWRead & 0xFF;
			CAM_INFO(CAM_SENSOR, "[OISFW]:%s 0x8000 = 0x%08x",
				__func__, FWRead);
			if (FW_version != OIS_CUR_FW_VERSION) {
				CAM_ERR(CAM_SENSOR,
					"[OISFW]:FW version check failed after update. retry.\n");
				rc = doFWupdate(cal_id, module_maker);
			} else {
				CAM_INFO(CAM_SENSOR,
					"[OISFW]: FW vserion verify pass.\n");
				break;
			}
		}
	}

	/* Restore the I2C slave address */
	s_ctrl->io_master_info.cci_client->sid =
		cci_client_sid_backup;

	CAM_INFO(CAM_SENSOR, "[OISFW]:%s rc = %d\n", __func__, rc);

	return rc;
}
EXPORT_SYMBOL_GPL(checkOISFWUpdate);

int GyroReCalibrate(struct camera_io_master *io_master_info,
	stReCalib *cal_result)
{
	int rc;
	stReCalib pReCalib;
	stGyroOffsetTbl StGyroOffsetTbl;
	g_io_master_info = io_master_info;
	if (g_io_master_info == NULL)
		return -EINVAL;

	RdGyroOffsetTbl(&StGyroOffsetTbl);
	rc = GyroReCalib(&pReCalib);
	memcpy(cal_result, &pReCalib, sizeof(stReCalib));
	if (rc != 0)
		return rc;

	CAM_INFO(CAM_SENSOR,
		"[OISCali]%d, FctryOffX = %d(0x%x), FctryOffY = %d(0x%x)",
		rc, cal_result->SsFctryOffX, cal_result->SsFctryOffX,
		cal_result->SsFctryOffY, cal_result->SsFctryOffY);
	CAM_INFO(CAM_SENSOR,
		"[OISCali]%d, RecalOffX = %d(0x%x), RecalOffY = %d(0x%x)",
		rc, cal_result->SsRecalOffX, cal_result->SsRecalOffX,
		cal_result->SsRecalOffY, cal_result->SsRecalOffY);
	CAM_INFO(CAM_SENSOR,
		"[OISCali]%d, DiffX = %d(0x%x), DiffY = %d(0x%x)",
		rc, cal_result->SsDiffX, cal_result->SsDiffX,
		cal_result->SsDiffY, cal_result->SsDiffY);

	if (abs(cal_result->SsRecalOffX) >= 0x600 ||
		abs(cal_result->SsRecalOffY) >= 0x600 ||
		abs(cal_result->SsDiffX) >= 0x1000 ||
		abs(cal_result->SsDiffY) >= 0x1000) {
		CAM_ERR(CAM_SENSOR,
			"[OISCali] Check failed.");
		return -EINVAL;
	}
	return rc;
}
EXPORT_SYMBOL_GPL(GyroReCalibrate);

int WrGyroOffsetData(struct camera_io_master *io_master_info,
	stReCalib *cal_result)
{
	int rc;
	stGyroOffsetTbl StGyroOffsetTbl;
	g_io_master_info = io_master_info;
	if (g_io_master_info == NULL)
		return -EINVAL;

	StGyroOffsetTbl.StAngle.SlOffsetX = (INT_32)cal_result->SsRecalOffX << 16;
	StGyroOffsetTbl.StAngle.SlOffsetY = (INT_32)cal_result->SsRecalOffY << 16;
	rc = WrGyroOffsetTbl(&StGyroOffsetTbl);
	return rc;
}
EXPORT_SYMBOL_GPL(WrGyroOffsetData);

int getFWVersion(struct cam_sensor_ctrl_t *s_ctrl)
{
	int       rc = 0;
	uint32_t  RamAddr, UlReadVal;
	unsigned short cci_client_sid_backup;

	if (s_ctrl->sensordata->slave_info.sensor_id != 0x363) {
		return -EINVAL;
	}

	/* Backup the I2C slave address */
	cci_client_sid_backup = s_ctrl->io_master_info.cci_client->sid;

	/* Replace the I2C slave address with OIS component */
	if (s_ctrl->sensordata->slave_info.sensor_id == 0x363) {
		s_ctrl->io_master_info.cci_client->sid =
			OIS_REARWIDE_I2C_ADDR_WRITE >> 1;
	}

	/* read FW version */
	RamAddr = 0x8000;
	rc = camera_io_dev_read(&s_ctrl->io_master_info, RamAddr,
		&UlReadVal, CAMERA_SENSOR_I2C_TYPE_WORD,
		CAMERA_SENSOR_I2C_TYPE_DWORD);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR, "[FW] read i2c failed");
	else {
		s_ctrl->ois_fw_ver = UlReadVal & 0xFF;
		s_ctrl->vcm_fw_ver = UlReadVal & 0xFF;
		CAM_INFO(CAM_SENSOR,
			"ois_fwver=0x%02x, vcm_fwver=0x%02x\n",
			s_ctrl->ois_fw_ver, s_ctrl->vcm_fw_ver);
	}

	/* Restore the I2C slave address */
	s_ctrl->io_master_info.cci_client->sid =
		cci_client_sid_backup;

	return rc;
}
EXPORT_SYMBOL_GPL(getFWVersion);

int GyroOffsetCorrect(struct camera_io_master *io_master_info, uint32_t gyro_correct_index)
{
	int rc = 0;
	uint32_t AddrGyroZ = 0;
	uint32_t AddrAccelX = 0;
	uint32_t AddrAccelY = 0;
	uint32_t AddrAccelZ = 0;
	uint32_t UlReadVal = 0;

	g_io_master_info = io_master_info;
	if (g_io_master_info == NULL)
		return -EINVAL;

	CAM_INFO(CAM_SENSOR,
		"[OISFW]:%s gyro_correct_index = %d\n",
			__func__, gyro_correct_index);

	if (gyro_correct_index > 0) {
		// Program Memory to read
		if (gyro_correct_index == 1) {
			AddrGyroZ = 0x0008469C;
			AddrAccelX = 0x00084698;
			AddrAccelY = 0x0008469A;
			AddrAccelZ = 0x0008469C;
		} else if (gyro_correct_index == 2)  {
			AddrGyroZ = 0x00084634;
			AddrAccelX = 0x00084630;
			AddrAccelY = 0x00084632;
			AddrAccelZ = 0x00084634;
		} else {
			CAM_INFO(CAM_SENSOR,
				"[OISFW]:%s index is not supported : %d\n",
					__func__, gyro_correct_index);
			return -EINVAL;
		}
	} else {
		CAM_INFO(CAM_SENSOR,
			"[OISFW]:%s index is out of range : %d\n",
				__func__, gyro_correct_index);
		return -EINVAL;
	}

	rc = checkHighLevelCommand(20);
	if (rc != 0) {
		CAM_ERR(CAM_SENSOR,
			"[OISFW]:%s checkHighLevelCommand failed = %d\n",
			__func__, rc);
		return -EINVAL;
	} else {
		// Gyro Offset Z
		RamWrite32A(0x3000, AddrGyroZ);
		RamRead32A(0x4000, &UlReadVal);
		UlReadVal = UlReadVal & 0xFFFF;
		UlReadVal = ((UlReadVal << 8) & 0xff00) | ((UlReadVal >> 8) & 0x00ff);
		RamWrite32A(0x03A8, UlReadVal);

		// Accel Offset X
		RamWrite32A(0x3000, AddrAccelX);
		RamRead32A(0x4000, &UlReadVal);
		UlReadVal = (UlReadVal & 0xFFFF0000) >> 16;
		UlReadVal = ((UlReadVal << 8) & 0xff00) | ((UlReadVal >> 8) & 0x00ff);
		RamWrite32A(0x0454, UlReadVal);

		// Accel Offset Y
		RamWrite32A(0x3000, AddrAccelY);
		RamRead32A(0x4000, &UlReadVal);
		UlReadVal = (UlReadVal & 0xFFFF0000) >> 16;
		UlReadVal = ((UlReadVal << 8) & 0xff00) | ((UlReadVal >> 8) & 0x00ff);
		RamWrite32A(0x0480, UlReadVal);

		// Accel Offset Z
		RamWrite32A(0x3000, AddrAccelZ);
		RamRead32A(0x4000, &UlReadVal);
		UlReadVal = (UlReadVal & 0xFFFF0000) >> 16;
		UlReadVal = ((UlReadVal << 8) & 0xff00) | ((UlReadVal >> 8) & 0x00ff);
		RamWrite32A(0x04AC, UlReadVal);
	}

	return rc;
}
EXPORT_SYMBOL_GPL(GyroOffsetCorrect);


MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Fw Update");
