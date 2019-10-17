#!/bin/sh
make clean
make
/usr/bin/avrdude -C/etc/avrdude/avrdude.conf -v -patmega32u4 -cstk500v1 -P/dev/ttyACM0 -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0xcb:m -Uhfuse:w:0xd8:m -Ulfuse:w:0xff:m
/usr/bin/avrdude -C/etc/avrdude/avrdude.conf -v -patmega32u4 -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:Arduino-Leonardo.hex:i -Ulock:w:0x2F:m
