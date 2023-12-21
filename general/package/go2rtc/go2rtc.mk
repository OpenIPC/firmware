################################################################################
#
# go2rtc
#
################################################################################

GO2RTC_VERSION = master
GO2RTC_SITE = $(call github,AlexxIT,go2rtc,$(GO2RTC_VERSION))
GO2RTC_LICENSE = MIT
GO2RTC_LICENSE_FILES = LICENSE

GO2RTC_INSTALL_TARGET = YES

GO2RTC_DEPENDENCIES = host-go host-upx
GO2RTC_GO_LDFLAGS = -s -w

define GO2RTC_BUILD_CMDS
	GOARCH=$$( \
		case $(BR2_ARCH) in \
			x86) echo 386 ;; \
			x86_64) echo amd64 ;; \
			arm) echo arm ;; \
			aarch64) echo arm64 ;; \
			mips) echo mips ;; \
			mipsel) echo mipsle ;; \
			*) echo "Unsupported architecture: $(BR2_ARCH)" && exit 1 ;; \
		esac \
	); \
	echo "Building for GOARCH: $$GOARCH"; \
		(cd $(@D); \
		CGO_ENABLED=0 \
		GOOS=linux \
		GOARCH=$$GOARCH \
		go build -ldflags "$(GO2RTC_GO_LDFLAGS)")
endef

define GO2RTC_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/go2rtc $(TARGET_DIR)/usr/bin/go2rtc
	upx --best --lzma $(TARGET_DIR)/usr/bin/go2rtc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/ $(GO2RTC_PKGDIR)/files/go2rtc.yaml
endef

$(eval $(golang-package))
