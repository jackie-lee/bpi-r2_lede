#
# YUGA GobiNet AT Connect dial-up
#

include $(TOPDIR)/rules.mk

PKG_NAME:=goatcon
PKG_VERSION:=1.0.0

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=YUGA Gobinet AT dial-up Connector
	MAINTAINER:=Jinkai li <lijk@synertone.net>
endef

define Package/$(PKG_NAME)/description
	This is use for dial-up YUGA 9x07 4G module
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Package/goatcon/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/goatcon $(1)/usr/bin/
endef

$(eval $(call BuildPackage,goatcon))
