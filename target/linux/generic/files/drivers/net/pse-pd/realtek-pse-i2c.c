// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pse-pd/pse.h>

#include "realtek-pse.h"

static int rtpse_i2c_send(struct rtpse_ctrl *pse, const struct rtpse_mcu_msg *req)
{
	struct i2c_client *client = to_i2c_client(pse->dev);

	/* Send opcode as SMBus command byte; remaining 11 bytes as block data */
	return i2c_smbus_write_i2c_block_data(client, req->opcode, RTPSE_MCU_MSG_SIZE - 1,
					      (u8 *)req + 1);
}

/*
 * The core has already waited RTPSE_MCU_RESPONSE_MS before calling us, so
 * the response is normally ready on the very first read. For commands the
 * MCU produces more slowly, keep polling at the typical response cadence
 * up to the worst-case ceiling.
 */
#define RTPSE_I2C_RETRY_MS	RTPSE_MCU_RESPONSE_MS
#define RTPSE_I2C_MAX_TRIES	(RTPSE_MCU_RESPONSE_MAX_MS / RTPSE_I2C_RETRY_MS)

static int rtpse_i2c_recv(struct rtpse_ctrl *pse,
			    const struct rtpse_mcu_msg *req,
			    struct rtpse_mcu_msg *resp)
{
	struct i2c_client *client = to_i2c_client(pse->dev);
	int tries, ret;

	for (tries = 0; tries < RTPSE_I2C_MAX_TRIES; tries++) {
		if (tries > 0)
			msleep(RTPSE_I2C_RETRY_MS);

		/* MCU needs 0x00 as command byte for read */
		ret = i2c_smbus_read_i2c_block_data(client, 0x00,
						    RTPSE_MCU_MSG_SIZE,
						    (u8 *)resp);
		if (ret < 0)
			return ret;
		if (ret == RTPSE_MCU_MSG_SIZE && resp->opcode == req->opcode)
			return 0;
	}

	return -ETIMEDOUT;
}

static const struct rtpse_transport_ops rtpse_i2c_transport_ops = {
	.send = rtpse_i2c_send,
	.recv = rtpse_i2c_recv,
};

static int rtpse_i2c_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct rtpse_ctrl *pse;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_WRITE_I2C_BLOCK |
						      I2C_FUNC_SMBUS_READ_I2C_BLOCK))
		return dev_err_probe(dev, -EOPNOTSUPP,
				     "adapter lacks SMBus I2C-block support\n");

	pse = devm_kzalloc(dev, sizeof(*pse), GFP_KERNEL);
	if (!pse)
		return -ENOMEM;

	pse->dev = dev;
	pse->transport = &rtpse_i2c_transport_ops;

	return rtpse_register(pse);
}

static const struct of_device_id rtpse_i2c_of_match[] = {
	RTPSE_OF_MATCH_ENTRIES
};
MODULE_DEVICE_TABLE(of, rtpse_i2c_of_match);

static const struct i2c_device_id rtpse_i2c_id[] = {
	{ "rtl8238b" },
	{ "rtl8239"  },
	{ "rtl8239c" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, rtpse_i2c_id);

static struct i2c_driver rtpse_i2c_driver = {
	.driver = {
		.name		= "realtek-pse-i2c",
		.of_match_table	= rtpse_i2c_of_match,
	},
	.id_table	= rtpse_i2c_id,
	.probe		= rtpse_i2c_probe,
};
module_i2c_driver(rtpse_i2c_driver);

MODULE_AUTHOR("Jonas Jelonek <jelonek.jonas@gmail.com>");
MODULE_DESCRIPTION("PSE driver for Realtek PoE MCU (I2C transport)");
MODULE_LICENSE("GPL");
