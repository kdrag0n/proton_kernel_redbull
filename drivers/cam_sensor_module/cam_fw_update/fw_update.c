#include "fw_update.h"

#define OIS_REARWIDE_I2C_ADDR_WRITE  0x76
#define OIS_REARTELE_I2C_ADDR_WRITE  0x78

int getFWVersion(struct cam_sensor_ctrl_t *s_ctrl)
{
	int       rc = 0;
	uint32_t  RamAddr, UlReadVal;
	unsigned short cci_client_sid_backup;

	if (s_ctrl->sensordata->slave_info.sensor_id != 0x363 &&
		s_ctrl->sensordata->slave_info.sensor_id != 0x481) {
		return -EINVAL;
	}

	/* Backup the I2C slave address */
	cci_client_sid_backup = s_ctrl->io_master_info.cci_client->sid;

	/* Replace the I2C slave address with OIS component */
	if (s_ctrl->sensordata->slave_info.sensor_id == 0x363) {
		s_ctrl->io_master_info.cci_client->sid =
			OIS_REARWIDE_I2C_ADDR_WRITE >> 1;
	} else {
		// s_ctrl->sensordata->slave_info.sensor_id == 0x481
		s_ctrl->io_master_info.cci_client->sid =
			OIS_REARTELE_I2C_ADDR_WRITE >> 1;
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