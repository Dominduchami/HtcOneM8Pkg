#!/bin/bash
cat BootShim/BootShim.bin workspace/Build/HtcOneM8/DEBUG_CLANGDWARF/FV/MSM8974_UEFI.fd >>ImageResources/HtcOneM8/bootpayload.bin

dtc -I dts -O dtb ImageResources/HtcOneM8/msm8974-htc-m8.dts -o ImageResources/HtcOneM8/msm8974-htc-m8.dtb

cat ImageResources/HtcOneM8/msm8974-htc-m8.dtb >>ImageResources/HtcOneM8/bootpayload.bin

# --ramdisk_offset 0x02008000 --tags_offset 0x01e00000
mkbootimg --kernel ImageResources/HtcOneM8/bootpayload.bin --base 0x00000000 --kernel_offset 0x00008000 -o ImageResources/HtcOneM8/uefi.img