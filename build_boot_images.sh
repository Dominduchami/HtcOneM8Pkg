#!/bin/bash
if [ $1 == 'OneM8' ]; then
    cat BootShim/BootShim.bin workspace/Build/HtcOneM8/DEBUG_GCC/FV/MSM8974_UEFI.fd >>ImageResources/HtcOneM8/bootpayload.bin

    #mkbootimg --kernel ImageResources/HtcOneM8/bootpayload.bin --base 0x11800000 --kernel_offset 0x00008000 -o ImageResources/HtcOneM8/uefi.img
else
    echo "Bootimages: Invalid platform"
fi