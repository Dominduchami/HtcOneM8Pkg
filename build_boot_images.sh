#!/bin/bash
#cat BootShim/BootShim.bin workspace/Build/HtcOneM8/DEBUG_CLANGDWARF/FV/MSM8974_UEFI.fd >>ImageResources/HtcOneM8/bootpayload.bin

cp workspace/Build/HtcOneM8/DEBUG_CLANGDWARF/FV/MSM8974_UEFI.fd ImageResources/HtcOneM8/bootpayload.bin

mkbootimg --kernel ImageResources/HtcOneM8/bootpayload.bin --base 0x00000000 --kernel_offset 0x00008000 --ramdisk_offset 0x02008000 --dtb ImageResources/HtcOneM8/qcdt.img -o ImageResources/HtcOneM8/uefi.img

# Create an elf for loading with BootShim
#llvm-objcopy -I binary -O elf32-littlearm --binary-architecture arm workspace/Build/HtcOneM8/DEBUG_CLANGDWARF/FV/MSM8974_UEFI.fd ImageResources/HtcOneM8/MSM8974_EFI.fd.elf
#ld.lld ImageResources/HtcOneM8/MSM8974_EFI.fd.elf -T FvWrapper.ld -o ImageResources/HtcOneM8/emmc_appsboot.mbn
#rm ImageResources/HtcOneM8/MSM8974_EFI.fd.elf