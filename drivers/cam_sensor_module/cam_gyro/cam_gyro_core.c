// SPDX-License-Identifier: GPL-2.0-only
/**
 * @brief Initialize ICM20690 cam gyro
 *
 **/

#include "cam_gyro_core.h"

#define ICM20690_I2C_ADDR_WRITE 0xD0

static struct gyro_ctrl_t *ctrl;

struct gyro_ctrl_t {
	struct mutex gyro_sensor_mutex;
	struct camera_io_master io_master_info;
	bool is_cci_init;
};

static int write_data_u8(uint32_t addr, uint32_t data)
{
	int rc;
	struct cam_sensor_i2c_reg_array i2c_reg_entry = {
		.reg_addr = addr,
		.reg_data = data,
		.delay = 0
	};
	struct cam_sensor_i2c_reg_setting i2c_reg_settings = {
		.addr_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.data_type = CAMERA_SENSOR_I2C_TYPE_BYTE,
		.size = 1,
		.delay = 0,
		.reg_setting = &i2c_reg_entry
	};

	rc = camera_io_dev_write(&(ctrl->io_master_info),
		&i2c_reg_settings);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR,
			"%s: write 0x%x failed", __func__, addr);

	return rc;
}

static int read_data_u8(uint32_t addr, uint32_t *data)
{
	int rc;

	rc = camera_io_dev_read(&(ctrl->io_master_info), addr,
		data, CAMERA_SENSOR_I2C_TYPE_BYTE,
		CAMERA_SENSOR_I2C_TYPE_BYTE);
	if (rc < 0)
		CAM_ERR(CAM_SENSOR,
			"%s: read 0x%x failed", __func__, addr);

	return rc;
}

int init_cam_gyro(void)
{
	int rc = 0;

	if (ctrl) {
		CAM_INFO(CAM_SENSOR,
				"%s cam gyro has been initialized.", __func__);
		return rc;
	}

	/* Create sensor control structure */
	ctrl = kzalloc(sizeof(struct gyro_ctrl_t), GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	ctrl->io_master_info.master_type = CCI_MASTER;
	ctrl->is_cci_init = false;

	/* Initialize cci_client */
	ctrl->io_master_info.cci_client = kzalloc(sizeof(
		struct cam_sensor_cci_client), GFP_KERNEL);
	if (!(ctrl->io_master_info.cci_client)) {
		rc = -ENOMEM;
		goto ctrl_free;
	}

	ctrl->io_master_info.cci_client->cci_i2c_master = MASTER_1;
	ctrl->io_master_info.cci_client->cci_device = CCI_DEVICE_0;
	ctrl->io_master_info.cci_client->retries = 3;
	ctrl->io_master_info.cci_client->id_map = 0;
	/* Gyro initial with 400k Hz I2C */
	ctrl->io_master_info.cci_client->i2c_freq_mode = I2C_FAST_MODE;
	ctrl->io_master_info.cci_client->sid =
		ICM20690_I2C_ADDR_WRITE >> 1;

	mutex_init(&ctrl->gyro_sensor_mutex);

	return rc;

ctrl_free:
	kfree(ctrl);
	return rc;
}
EXPORT_SYMBOL_GPL(init_cam_gyro);

int enable_cam_gyro(void)
{
	int rc;
	uint32_t gyro_value = 0;

	mutex_lock(&ctrl->gyro_sensor_mutex);
	if (!ctrl->is_cci_init) {
		rc = camera_io_init(&(ctrl->io_master_info));
		if (rc < 0) {
			CAM_ERR(CAM_SENSOR,
				"%s cci_init failed: rc: %d", __func__, rc);
			goto error_out;
		}
		ctrl->is_cci_init = true;
	}

	/* wake-up chip & select clock source */
	rc = write_data_u8(0x6B, 0x01);
	if (rc < 0)
		goto error_out;

	/* read who_am_i gyro value */
	rc = read_data_u8(0x75, &gyro_value);
	if (rc < 0)
		goto error_out;

	/* initialize settings of OIS Gyro/Accel */
	if (gyro_value != 0x20) {
		rc = -EFAULT;
		CAM_ERR(CAM_SENSOR, "[ICM20690] failed to initial gyro");
		goto error_out;
	}

	/* FSR=±8g for OIS ACCEL */
	rc = write_data_u8(0x1C, 0x12);
	if (rc < 0)
		goto error_out;
	/* A_DLPF_CFG=6(BW=5.05Hz) */
	rc = write_data_u8(0x1D, 0x06);
	if (rc < 0)
		goto error_out;
	/* FSR=±500dps for OIS gyro, BW250Hz DLPF,ODR=8KHz */
	rc = write_data_u8(0x68, 0x28);
	if (rc < 0)
		goto error_out;
	/* ACCEL_FCHOICE_OIS_B=01, ODR=1Khz */
	rc = write_data_u8(0x69, 0x10);
	if (rc < 0)
		goto error_out;
	/* XYZ-GYRO enable, XYZ-ACCEL enable */
	rc = write_data_u8(0x6C, 0x00);
	if (rc < 0)
		goto error_out;
	/* AUX bus is OIS SPI */
	rc = write_data_u8(0x70, 0x02);
	if (rc < 0)
		goto error_out;
	/* PWR_MGMT_2: xyz-gyro & accel enable */
	rc = write_data_u8(0x6C, 0x00);
	if (rc < 0)
		goto error_out;

error_out:
	mutex_unlock(&ctrl->gyro_sensor_mutex);
	return rc;
}
EXPORT_SYMBOL_GPL(enable_cam_gyro);

int disable_cam_gyro(void)
{
	int rc, is_error;

	mutex_lock(&ctrl->gyro_sensor_mutex);
	/* Disable OIS Gyro/Accel */
	rc = write_data_u8(0x6C, 0x3F);
	if (rc < 0) {
		CAM_ERR(CAM_SENSOR,
			"[ICM20690] failed to disable gyro");
		mutex_unlock(&ctrl->gyro_sensor_mutex);
		return rc;
	}

	if (ctrl->is_cci_init) {
		is_error = camera_io_release(&(ctrl->io_master_info));
		if (is_error < 0) {
			rc = is_error;
			CAM_ERR(CAM_SENSOR,
				"%s cci_release failed: rc: %d", __func__, rc);
		} else
			ctrl->is_cci_init = false;
	}
	mutex_unlock(&ctrl->gyro_sensor_mutex);
	return rc;
}
EXPORT_SYMBOL_GPL(disable_cam_gyro);

void release_cam_gyro(void)
{
	if (ctrl) {
		mutex_destroy(&ctrl->gyro_sensor_mutex);
		kfree(ctrl->io_master_info.cci_client);
		kfree(ctrl);
	}
}
EXPORT_SYMBOL_GPL(release_cam_gyro);

MODULE_DESCRIPTION("Invensense ICM20690 Gyro sensor");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Nick Chung <nickchung@google.com>");