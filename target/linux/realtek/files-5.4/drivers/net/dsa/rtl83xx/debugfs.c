// SPDX-License-Identifier: GPL-2.0-only

#include <linux/debugfs.h>

#include <asm/mach-rtl838x/mach-rtl83xx.h>
#include "rtl838x.h"

#define RTL838X_DRIVER_NAME "rtl838x"

static const struct debugfs_reg32 port_ctrl_regs[] = {
	{ .name = "port_isolation", .offset = RTL838X_PORT_ISO_CTRL(0), },
	{ .name = "mac_force_mode", .offset = RTL838X_MAC_FORCE_MODE_CTRL, },
};

void rtl838x_dbgfs_cleanup(struct rtl838x_switch_priv *priv)
{
	debugfs_remove_recursive(priv->dbgfs_dir);

//	kfree(priv->dbgfs_entries);
}

static int rtl838x_dbgfs_port_init(struct dentry *parent, struct rtl838x_switch_priv *priv,
				   int port)
{
	struct dentry *port_dir;
	struct debugfs_regset32 *port_ctrl_regset;

	port_dir = debugfs_create_dir(priv->ports[port].dp->name, parent);

	debugfs_create_x32("rate_uc", 0644, port_dir,
			    (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_UC(port)));

	debugfs_create_x32("rate_mc", 0644, port_dir,
			    (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_BC(port)));

	debugfs_create_x32("rate_bc", 0644, port_dir,
			    (u32 *)(RTL838X_SW_BASE + RTL838X_STORM_CTRL_PORT_BC(port)));

	debugfs_create_u32("id", 0444, port_dir, &priv->ports[port].dp->index);


	debugfs_create_x32("vlan_port_tag_sts_ctrl", 0644, port_dir,
			   (u32 *)(RTL838X_SW_BASE + RTL838X_VLAN_PORT_TAG_STS_CTRL(port)));

	port_ctrl_regset = devm_kzalloc(priv->dev, sizeof(*port_ctrl_regset), GFP_KERNEL);
	if (!port_ctrl_regset)
		return -ENOMEM;

	port_ctrl_regset->regs = port_ctrl_regs;
	port_ctrl_regset->nregs = ARRAY_SIZE(port_ctrl_regs);
	port_ctrl_regset->base = RTL838X_SW_BASE + (port << 2);
	debugfs_create_regset32("port_ctrl", 0400, port_dir, port_ctrl_regset);

	return 0;
}

void rtl838x_serdes_init(struct dentry *dir, struct rtl838x_switch_priv *priv)
{
	debugfs_create_x32("RTL8380_SDS4_INTF_CTRL", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0x20));
	debugfs_create_x32("RTL8380_SDS4_REG0", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEF80));
	debugfs_create_x32("RTL8380_SDS4_REG1", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEF84));
	debugfs_create_x32("RTL8380_SDS4_REG2", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEF88));
	debugfs_create_x32("RTL8380_SDS4_REG3", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEF8C));
	debugfs_create_x32("RTL8380_SDS4_REG4", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEF90));
	debugfs_create_x32("RTL8380_SDS4_REG5", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEF94));
	debugfs_create_x32("RTL8380_SDS4_REG6", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEF98));
	debugfs_create_x32("RTL8380_SDS4_REG7", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEF9C));
	debugfs_create_x32("RTL8380_SDS4_REG8", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFA0));
	debugfs_create_x32("RTL8380_SDS4_REG9", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFA4));
	debugfs_create_x32("RTL8380_SDS4_REG10", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFA8));
	debugfs_create_x32("RTL8380_SDS4_REG11", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFAC));
	debugfs_create_x32("RTL8380_SDS4_REG12", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFB0));
	debugfs_create_x32("RTL8380_SDS4_REG13", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFB4));
	debugfs_create_x32("RTL8380_SDS4_REG14", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFB8));
	debugfs_create_x32("RTL8380_SDS4_REG15", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFBC));
	debugfs_create_x32("RTL8380_SDS4_REG16", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFC0));
	debugfs_create_x32("RTL8380_SDS4_REG17", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFC4));
	debugfs_create_x32("RTL8380_SDS4_REG18", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFC8));
	debugfs_create_x32("RTL8380_SDS4_REG19", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFCC));
	debugfs_create_x32("RTL8380_SDS4_REG20", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFD0));
	debugfs_create_x32("RTL8380_SDS4_REG21", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFD4));
	debugfs_create_x32("RTL8380_SDS4_REG22", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFD8));
	debugfs_create_x32("RTL8380_SDS4_REG23", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFDC));
	debugfs_create_x32("RTL8380_SDS4_REG24", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFE0));
	debugfs_create_x32("RTL8380_SDS4_REG25", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFE4));
	debugfs_create_x32("RTL8380_SDS4_REG26", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFE8));
	debugfs_create_x32("RTL8380_SDS4_REG27", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFEC));
	debugfs_create_x32("RTL8380_SDS4_REG28", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFF0));
	debugfs_create_x32("RTL8380_SDS4_REG29", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFF4));
	debugfs_create_x32("RTL8380_SDS4_REG30", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xEFF8));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG0", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF100));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG1", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF104));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG2", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF108));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG3", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF10C));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG4", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF110));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG5", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF114));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG6", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF118));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG7", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF11C));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG8", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF120));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG9", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF124));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG10", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF128));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG11", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF12C));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG12", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF130));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG13", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF134));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG14", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF138));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG15", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF13C));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG16", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF140));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG17", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF144));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG18", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF148));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG19", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF14C));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG20", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF150));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG21", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF154));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG22", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF158));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG23", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF15C));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG24", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF160));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG25", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF164));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG26", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF168));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG27", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF16C));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG28", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF170));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG29", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF174));
	debugfs_create_x32("RTL8380_SDS4_FIB_EXT_REG30", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF178));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG0", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF780));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG1", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF784));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG2", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF788));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG3", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF78C));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG4", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF790));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG5", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF794));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG6", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF798));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG7", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF79C));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG8", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7A0));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG9", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7A4));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG10", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7A8));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG11", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7AC));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG12", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7B0));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG13", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7B4));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG14", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7B8));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG15", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7BC));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG16", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7C0));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG17", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7C4));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG18", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7C8));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG19", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7CC));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG20", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7D0));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG21", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7D4));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG22", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7D8));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG23", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7DC));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG24", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7E0));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG25", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7E4));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG26", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7E8));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG27", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7EC));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG28", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7F0));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG29", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7F4));
	debugfs_create_x32("RTL8380_SDS4_EXT_REG30", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF7F8));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG0", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF800));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG1", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF804));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG2", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF808));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG3", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF80C));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG4", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF810));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG5", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF814));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG6", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF818));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG7", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF81C));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG8", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF820));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG9", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF824));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG10", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF828));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG11", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF82C));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG12", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF830));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG13", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF834));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG14", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF838));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG15", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF83C));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG16", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF840));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG17", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF844));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG18", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF848));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG19", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF84C));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG20", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF850));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG21", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF854));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG22", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF858));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG23", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF85C));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG24", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF860));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG25", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF864));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG26", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF868));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG27", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF86C));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG28", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF870));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG29", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF874));
	debugfs_create_x32("RTL8380_SDS4_FIB_REG30", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF878));

}

void rtl838x_dbgfs_init(struct rtl838x_switch_priv *priv)
{
	struct dentry *rtl838x_dir;
	struct dentry *port_dir;
	struct debugfs_regset32 *port_ctrl_regset;
	int ret, i;

	rtl838x_dir = debugfs_lookup(RTL838X_DRIVER_NAME, NULL);
	if (!rtl838x_dir)
		rtl838x_dir = debugfs_create_dir(RTL838X_DRIVER_NAME, NULL);

	priv->dbgfs_dir = rtl838x_dir;

	debugfs_create_u32("soc", 0444, rtl838x_dir,
			   (u32 *)(RTL838X_SW_BASE + RTL838X_MODEL_NAME_INFO));

	/* Create one directory per port */
	for (i = 0; i < priv->cpu_port; i++) {
		if (priv->ports[i].phy) {
			pr_debug("debugfs, port %d\n", i);
			ret = rtl838x_dbgfs_port_init(rtl838x_dir, priv, i);
			if (ret)
				goto err;
		}
	}

	/* Create directory for CPU-port */
	port_dir = debugfs_create_dir("cpu_port", rtl838x_dir);	port_ctrl_regset = devm_kzalloc(priv->dev, sizeof(*port_ctrl_regset), GFP_KERNEL);
	if (!port_ctrl_regset) {
		ret = -ENOMEM;
		goto err;
	}

	port_ctrl_regset->regs = port_ctrl_regs;
	port_ctrl_regset->nregs = ARRAY_SIZE(port_ctrl_regs);
	port_ctrl_regset->base = RTL838X_SW_BASE + (priv->cpu_port << 2);
	debugfs_create_regset32("port_ctrl", 0400, port_dir, port_ctrl_regset);
	debugfs_create_u8("id", 0444, port_dir, &priv->cpu_port);

	port_dir = debugfs_create_dir("serdes", rtl838x_dir);
	rtl838x_serdes_init(port_dir, priv);
	return;
err:
	rtl838x_dbgfs_cleanup(priv);
}
