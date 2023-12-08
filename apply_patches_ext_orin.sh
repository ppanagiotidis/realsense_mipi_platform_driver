#!/bin/bash
set -e

if [[ $# < 1 ]]; then
    echo "apply_patches_ext.sh [--one-cam | --dual-cam] source_dir [JetPack_version]"
    exit 1
fi

if [[ "$2" != "5.1.2" ]]; then
    echo "JetPack version not 5.1.2. Exiting..."
    exit 1
fi

# Default to single camera DT for JetPack 5.0.2
# single - jp5 [default] single cam GMSL board
# dual - dual cam GMSL board SC20220126
JP5_D4XX_DTSI="tegra234-camera-d4xx-single.dtsi"

DEVDIR=$(cd `dirname $0` && pwd)

. $DEVDIR/scripts/setup-common "$2"

cd "$DEVDIR"

apply_external_patches() {
cat ${PWD}/$2/$JETPACK_VERSION/* | patch -p1 --directory=${PWD}/$1/$2/
}

apply_external_patches $1 kernel/nvidia
apply_external_patches $1 $KERNEL_DIR
apply_external_patches $1 hardware/nvidia/platform/t23x/concord/kernel-dts

# For a common driver for JP4 + JP5 we override the i2c driver and ignore the previous that was created from patches
cp $DEVDIR/kernel/realsense/d4xx.c $DEVDIR/$1/kernel/nvidia/drivers/media/i2c/
cp $DEVDIR/hardware/realsense/$JP5_D4XX_DTSI $DEVDIR/$1/hardware/nvidia/platform/t23x/concord/kernel-dts/cvb/tegra234-camera-d4xx.dtsi
