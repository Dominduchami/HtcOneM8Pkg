#!/bin/bash
if [ $1 == 'OneM8' ]; then
    # TODO: Add dtb
    cat BootShim/BootShim.bin workspace/Build/HtcOneM8/DEBUG_GCC/FV/MSM8974_UEFI.fd >>ImageResources/HtcOneM8/bootpayload.bin

    # --kernel_offset 0x00008000 --ramdisk_offset 0x02008000 --tags_offset 0x01e00000
    mkbootimg --kernel ImageResources/HtcOneM8/bootpayload.bin --base 0x00000000 --kernel_offset 0x00008000 -o ImageResources/HtcOneM8/uefi.img
else
    echo "Bootimages: Invalid platform"
fi