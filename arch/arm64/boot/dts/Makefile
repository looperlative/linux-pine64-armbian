dtb-$(CONFIG_ARCH_VEXPRESS) += rtsm_ve-aemv8a.dtb foundation-v8.dtb \
				fvp-base-gicv2-psci.dtb
dtb-$(CONFIG_ARCH_VEXPRESS) += juno.dtb
dtb-$(CONFIG_ARCH_XGENE) += apm-mustang.dtb
dtb-$(CONFIG_ARCH_SUN50I) += sun50iw1p1-fpga.dtb \
				sun50iw1p1-soc.dtb \
				sun50iw1p1-orangepiwin.dtb \
				sun50iw1p1-bananapim64.dtb \
				sun50iw1p1-olinuxino-a64.dtb \
				sun50iw1p1-teres.dtb \
				sun50iw1p1-nanopim64.dtb \
				sun50iw1p1-pine64.dtb \
				sun50iw1p1-pine64-plus.dtb \
				sun50iw1p1-pine64-pinebook.dtb \
				sun50iw1p1-pine64so.dtb

targets += dtbs

DTB_NAMES := $(subst $\",,$(CONFIG_BUILD_ARM64_APPENDED_DTB_IMAGE_NAMES))
ifneq ($(DTB_NAMES),)
DTB_LIST := $(addsuffix .dtb,$(DTB_NAMES))
else
DTB_LIST := $(dtb-y)
endif
targets += $(DTB_LIST)

dtbs: $(addprefix $(obj)/, $(DTB_LIST))

clean-files := dts/*.dtb *.dtb
