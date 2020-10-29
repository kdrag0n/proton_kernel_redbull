ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
dtbo-bramble += lito-v2-bramble-pvt-overlay.dtbo
lito-v2-bramble-pvt-overlay.dtbo-base := qcom-base/lito-v2.dtb

dtbo-$(CONFIG_BOARD_BRAMBLE) += lito-v2-bramble_m-pvt-overlay.dtbo
lito-v2-bramble_m-pvt-overlay.dtbo-base := qcom-base/lito-v2.dtb

dtbo-redfin += lito-v2-redfin-pvt-overlay.dtbo
lito-v2-redfin-pvt-overlay.dtbo-base := qcom-base/lito-v2.dtb
endif

always          := $(dtb-y)
subdir-y        := $(dts-dirs)
clean-files     := *.dtb
