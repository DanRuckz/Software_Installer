#!/bin/bash
mv /mobileye/system/hd_scripts/bogdan/software-installer/software-installer \
/mobileye/system/hd_scripts/bogdan/software-installer/tmp/`cat /dev/urandom |tr -cd 'a-f0-9' | head -c 32`
cp -r assets/ software-installer /mobileye/system/hd_scripts/bogdan/software-installer/
