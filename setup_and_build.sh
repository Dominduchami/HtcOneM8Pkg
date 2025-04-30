#!/bin/bash
# based on the instructions from edk2-platform
set -e
rm -rf ImageResources/*.img ImageResources/Tools/*.bin BootShim/*.bin
export PACKAGES_PATH=$PWD/../edk2:$PWD/../edk2-platforms:$PWD
export WORKSPACE=$PWD/workspace
. ../edk2/edksetup.sh

rm -rf workspace/Build

NUM_CPUS=$((`getconf _NPROCESSORS_ONLN` + 2))
build -n $NUM_CPUS -a ARM -t CLANGDWARF -p Platforms/HtcOneM8/HtcOneM8Pkg.dsc -b DEBUG

chmod +x build_boot_shim.sh
./build_boot_shim.sh

chmod +x build_boot_images.sh
./build_boot_images.sh
