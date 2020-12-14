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
	/* common */
	debugfs_create_x32("RTL8380_SDS_MODE_SEL", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0x28));
	debugfs_create_x32("RTL8380_SDS_CFG_REG", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0x34));
	debugfs_create_x32("RTL8380_INT_RW_CTRL", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0x58));
	debugfs_create_x32("RTL8380_INT_MODE_CTRL", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0x5C));
	debugfs_create_x32("RTL8380_SMI_GLB_CTRL", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xA100));

	/* SDS4 */
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

	/* SDS5 */
	debugfs_create_x32("RTL8380_SDS5_INTF_CTRL", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0x24));
	debugfs_create_x32("RTL8380_SDS5_REG0", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF180));
	debugfs_create_x32("RTL8380_SDS5_REG1", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF184));
	debugfs_create_x32("RTL8380_SDS5_REG2", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF188));
	debugfs_create_x32("RTL8380_SDS5_REG3", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF18C));
	debugfs_create_x32("RTL8380_SDS5_REG4", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF190));
	debugfs_create_x32("RTL8380_SDS5_REG5", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF194));
	debugfs_create_x32("RTL8380_SDS5_REG6", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF198));
	debugfs_create_x32("RTL8380_SDS5_REG7", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF19C));
	debugfs_create_x32("RTL8380_SDS5_REG8", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1A0));
	debugfs_create_x32("RTL8380_SDS5_REG9", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1A4));
	debugfs_create_x32("RTL8380_SDS5_REG10", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1A8));
	debugfs_create_x32("RTL8380_SDS5_REG11", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1AC));
	debugfs_create_x32("RTL8380_SDS5_REG12", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1B0));
	debugfs_create_x32("RTL8380_SDS5_REG13", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1B4));
	debugfs_create_x32("RTL8380_SDS5_REG14", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1B8));
	debugfs_create_x32("RTL8380_SDS5_REG15", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1BC));
	debugfs_create_x32("RTL8380_SDS5_REG16", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1C0));
	debugfs_create_x32("RTL8380_SDS5_REG17", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1C4));
	debugfs_create_x32("RTL8380_SDS5_REG18", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1C8));
	debugfs_create_x32("RTL8380_SDS5_REG19", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1CC));
	debugfs_create_x32("RTL8380_SDS5_REG20", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1D0));
	debugfs_create_x32("RTL8380_SDS5_REG21", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1D4));
	debugfs_create_x32("RTL8380_SDS5_REG22", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1D8));
	debugfs_create_x32("RTL8380_SDS5_REG23", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1DC));
	debugfs_create_x32("RTL8380_SDS5_REG24", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1E0));
	debugfs_create_x32("RTL8380_SDS5_REG25", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1E4));
	debugfs_create_x32("RTL8380_SDS5_REG26", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1E8));
	debugfs_create_x32("RTL8380_SDS5_REG27", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1EC));
	debugfs_create_x32("RTL8380_SDS5_REG28", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1F0));
	debugfs_create_x32("RTL8380_SDS5_REG29", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1F4));
	debugfs_create_x32("RTL8380_SDS5_REG30", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF1F8));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG0", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF300));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG1", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF304));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG2", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF308));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG3", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF30C));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG4", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF310));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG5", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF314));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG6", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF318));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG7", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF31C));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG8", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF320));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG9", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF324));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG10", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF328));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG11", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF32C));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG12", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF330));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG13", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF334));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG14", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF338));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG15", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF33C));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG16", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF340));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG17", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF344));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG18", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF348));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG19", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF34C));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG20", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF350));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG21", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF354));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG22", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF358));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG23", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF35C));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG24", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF360));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG25", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF364));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG26", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF368));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG27", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF36C));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG28", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF370));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG29", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF374));
	debugfs_create_x32("RTL8380_SDS5_FIB_EXT_REG30", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF378));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG0", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF880));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG1", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF884));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG2", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF888));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG3", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF88C));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG4", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF890));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG5", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF894));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG6", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF898));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG7", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF89C));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG8", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8A0));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG9", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8A4));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG10", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8A8));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG11", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8AC));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG12", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8B0));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG13", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8B4));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG14", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8B8));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG15", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8BC));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG16", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8C0));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG17", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8C4));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG18", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8C8));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG19", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8CC));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG20", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8D0));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG21", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8D4));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG22", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8D8));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG23", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8DC));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG24", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8E0));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG25", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8E4));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG26", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8E8));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG27", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8EC));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG28", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8F0));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG29", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8F4));
	debugfs_create_x32("RTL8380_SDS5_EXT_REG30", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF8F8));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG0", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF900));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG1", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF904));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG2", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF908));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG3", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF90C));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG4", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF910));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG5", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF914));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG6", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF918));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG7", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF91C));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG8", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF920));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG9", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF924));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG10", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF928));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG11", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF92C));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG12", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF930));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG13", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF934));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG14", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF938));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG15", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF93C));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG16", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF940));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG17", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF944));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG18", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF948));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG19", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF94C));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG20", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF950));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG21", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF954));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG22", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF958));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG23", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF95C));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG24", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF960));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG25", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF964));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG26", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF968));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG27", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF96C));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG28", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF970));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG29", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF974));
	debugfs_create_x32("RTL8380_SDS5_FIB_REG30", 0644, dir, (u32 *)(RTL838X_SW_BASE + 0xF978));

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
