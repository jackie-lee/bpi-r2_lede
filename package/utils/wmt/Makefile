#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=wmt
#PKG_RELEASE:=$(LINUX_VERSION)
PKG_VERSION:=1.0.0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=wmt utility
  VERSION:=$(LINUX_VERSION)
  MAINTAINER:=Jinkai li <lijk@synertone.net>
endef

define Package/$(PKG_NAME)/description
  wmt utility for load MT6625L module.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

#define Build/Compile
#	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/spidev_test \
#	$(PKG_BUILD_DIR)/spidev_test.c
#endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/stp_uart_launcher $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wmt_loader $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wmt_loopback $(1)/usr/bin/
	mkdir -p $(1)/system/etc/firmware
	$(CP) ./config/WMT_SOC.cfg $(1)/system/etc/firmware
	mkdir -p $(1)/etc/firmware/
	$(CP) -r ./firmware/* $(1)/etc/firmware/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
