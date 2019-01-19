This is the patched Linux kernel for I2S audio on a Pine64+.  It is based on the fully
patched kernel from Armbian.  Other Pine boards will need similar device tree changes, but
I haven't touched the other DTS files.  Feel free to modify for yourself.

Partially tested with CS4270 codec and fully tested with WM8731 codec on the mikroelektronika
board. S16_LE and S32_LE playback audio on the WM8731 board.

S24_LE should work, but I don't currently have an application to test it.  S24_3LE is only
supported using "plughw:" instead of "hw:".  But this doesn't produce audio for me. I
suspect that Armbian isn't installing something necessary or there is bug in the audio
system because S24_3LE requires software translation to S24_LE before it can be played.
For my current application, I don't care.  So, I'll stick to the modes that can be directly
handled by the hardware.

Command to configure this kernel
make ARCH=arm64 CROSS_COMPILE="ccache aarch64-linux-gnu-" cs4270_i2s_defconfig

Command I use to build this kernel:
make ARCH=arm64 CROSS_COMPILE="ccache aarch64-linux-gnu-" Image modules dtbs

Command I use to build debs for this kernel:
make ARCH=arm64 CROSS_COMPILE="ccache aarch64-linux-gnu-" ARCH=arm64 KDEB_PKGVERSION=5.71 \
LOCALVERSION=-pine64 DEBFULLNAME="Bob Amstadt" DEBEMAIL="bob@looperlative.com" deb-pkg