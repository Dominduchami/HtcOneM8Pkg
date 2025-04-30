#!/bin/bash
#cat BootShim/BootShim.bin workspace/Build/HtcOneM8/DEBUG_CLANGDWARF/FV/MSM8974_UEFI.fd >>ImageResources/HtcOneM8/bootpayload.bin
cp workspace/Build/HtcOneM8/DEBUG_CLANGDWARF/FV/MSM8974_UEFI.fd ImageResources/HtcOneM8/bootpayload.bin

dtc -I dts -O dtb ImageResources/HtcOneM8/msm8974-htc-m8.dts -o ImageResources/HtcOneM8/msm8974-htc-m8.dtb

#cat ImageResources/HtcOneM8/msm8974-htc-m8.dtb >>ImageResources/HtcOneM8/bootpayload.bin

# for lk2nd
#mkbootimg --kernel ImageResources/HtcOneM8/bootpayload.bin.gz --base 0x00000000 --kernel_offset 0x00008000 --ramdisk_offset 0x02008000 --dtb ImageResources/HtcOneM8/msm8974-htc-m8.dtb -o ImageResources/HtcOneM8/lk2nd_uefi.img

# --ramdisk_offset 0x02008000 --tags_offset 0x01e00000
mkbootimg --kernel ImageResources/HtcOneM8/bootpayload.bin --base 0x00000000 --kernel_offset 0x00008000 --ramdisk_offset 0x02008000 --dtb ImageResources/HtcOneM8/qcdt.img -o ImageResources/HtcOneM8/uefi.img

#--kernel_offset 0x00008000 --ramdisk_offset 0x02008000 --dt device/htc/m8/recovery/kernel/dt.img --tags_offset 0x01e00000

# Create an elf for loading with BootShim
llvm-objcopy -I binary -O elf32-littlearm --binary-architecture arm workspace/Build/HtcOneM8/DEBUG_CLANGDWARF/FV/MSM8974_UEFI.fd ImageResources/HtcOneM8/MSM8974_EFI.fd.elf
ld.lld ImageResources/HtcOneM8/MSM8974_EFI.fd.elf -T FvWrapper.ld -o ImageResources/HtcOneM8/emmc_appsboot.mbn
rm ImageResources/HtcOneM8/MSM8974_EFI.fd.elf