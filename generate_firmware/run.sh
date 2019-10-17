#!/bin/sh
rm -rf firm_encrypted.hex
size=$(size -d -A firmware.hex | grep "Total" | awk  '{print $2}')
make
./comm $size
