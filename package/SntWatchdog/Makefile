#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=SntWatchdog
PKG_VERSION:=1.1.1

include $(INCLUDE_DIR)/package.mk

define Package/SntWatchdog
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=
  TITLE:=SntWatchdog
endef

define Package/SntWatchdog/description
	Synertone Intelligent Gateway Base Platform software watchdog.
endef

define Build/Compile
endef

define Package/SntWatchdog/install
	$(CP) ./files/* $(1)
		
	chmod 755 $(1)/etc/init.d/SntWatchdog 
	$(INSTALL_DIR) $(1)/etc/rc.d/ 
	$(LN) -s /etc/init.d/SntWatchdog  $(1)/etc/rc.d/S99SntWatchdog 
endef

$(eval $(call BuildPackage,SntWatchdog))
