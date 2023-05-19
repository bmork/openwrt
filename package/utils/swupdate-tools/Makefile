include $(TOPDIR)/rules.mk

PKG_NAME:=swupdate-tools
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/swupdate-tools
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt SWUpdate tools
  DEPENDS:=+@BUSYBOX_DEFAULT_CPIO
endef

define Package/swupdate-tools/description
  Simple tools to handle SWUpdate images, conforming to
  https://sbabic.github.io/swupdate/

  swupdate2sysupgrade - extract one kernel and one rootfs from a
    SWUpdate image and create a basic sysupgrade tar file.

  swupdate - converts the image and attempts to run sysupgrade
    with the result
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/swupdate-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) ./files/swupdate2sysupgrade $(1)/usr/sbin/
	$(LN) swupdate2sysupgrade $(1)/usr/sbin/swupdate
endef

$(eval $(call BuildPackage,swupdate-tools))
