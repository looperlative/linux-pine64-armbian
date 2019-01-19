This is the patched Linux kernel for I2S audio on a Pine64+.  It is based on the fully patched kernel from Armbian.

Partially tested with CS4270 codec and fully tested with WM8731 codec on the mikroelektronika board.  
S24_LE doesn't produce any audible playback, but haven't debugged the issue, yet.  S16_LE does playback 
audio on the WM8731 board.

