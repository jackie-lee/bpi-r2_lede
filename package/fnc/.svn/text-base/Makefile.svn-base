include $(TOPDIR)/rules.mk

PKG_NAME:=fnc
PKG_RELEASE:=1.0.1
PKG_FIXUP:=autoreconf

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_RELEASE)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
SECTION:=utils
CATEGORY:=Utilities
TITLE:=fnc
DEPENDS:=+libsodium +libev +libpcap
endef

define Package/$(PKG_NAME)/description
	fnc
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(1)/bin/	
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
