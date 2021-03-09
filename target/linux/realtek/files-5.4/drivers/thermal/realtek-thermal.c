// SPDX-License-Identifier: GPL-2.0-only
/*
 * Realtek thermal sensor driver
 *
 * Copyright (C) 2021 Bjørn Mork <bjorn@mork.no>>
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include "thermal_hwmon.h"

#define RTL8380_THERMAL_METER_CTRL0	0x0
#define RTL8380_THERMAL_METER_CTRL1	0x4
#define RTL8380_THERMAL_METER_CTRL2	0x8
#define RTL8380_THERMAL_METER_RESULT	0xc

#define RTL8380_FLAG_INIT	BIT(0)

struct realtek_thermal_priv {
	void __iomem *base;
	u32 flags;
};

static void rtl838x_thermal_init(struct realtek_thermal_priv *priv)
{

	u32 val = readl(priv->base + RTL8380_THERMAL_METER_CTRL0);

	val |= BIT(0);
	writel(val, priv->base + RTL8380_THERMAL_METER_CTRL0);
}

static int rtl838x_get_temp(void *arg, int *res)
{
	struct realtek_thermal_priv *priv = arg;
	u32 val;

	if (!(priv->flags & RTL8380_FLAG_INIT))
		rtl838x_thermal_init(priv);

	val = readl(priv->base + RTL8380_THERMAL_METER_RESULT);
	if (val & BIT(8))
		*res = (val & 0x3f) * 1000;
	else
		return -EAGAIN;
	return 0;
}

static const struct thermal_zone_of_device_ops rtl838x_ops = {
	.get_temp = rtl838x_get_temp,
};

static int realtek_thermal_probe(struct platform_device *pdev)
{
	struct realtek_thermal_priv *priv;
	struct thermal_zone_device *tzdev;
	struct resource *res;
	int ret;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

// FIXME: use devm_platform_get_and_ioremap_resource(pdev, 0, NULL); (from biot)
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	platform_set_drvdata(pdev, priv);
	tzdev = devm_thermal_zone_of_sensor_register(&pdev->dev, 0, priv,
						     device_get_match_data(&pdev->dev));
        if (IS_ERR(tzdev)) {
                ret = PTR_ERR(tzdev);
                dev_err(&pdev->dev, "Failed to register sensor: %d\n", ret);
                return ret;
        }

        if (devm_thermal_add_hwmon_sysfs(tzdev))
                dev_warn(&pdev->dev, "Failed to add hwmon sysfs attributes\n");

	return 0;
}

static const struct of_device_id realtek_sensor_ids[] = {
	{ .compatible = "realtek,rtl8380-thermal", .data = &rtl838x_ops, },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, realtek_sensor_ids);

static struct platform_driver realtek_thermal_driver = {
	.probe	= realtek_thermal_probe,
	.driver	= {
		.name		= "realtek-thermal",
		.of_match_table	= realtek_sensor_ids,
	},
};

module_platform_driver(realtek_thermal_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bjørn Mork <bjorn@mork.no>");
MODULE_DESCRIPTION("Realtek temperature sensor");
