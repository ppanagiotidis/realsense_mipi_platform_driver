# If building kernel on arm, install libssl-dev:native

# Download Jetson Source for 5.1.2

mkdir -p l4t-gcc/5.1.2
cd ./l4t-gcc/5.1.2
wget <https://developer.nvidia.com/embedded/jetson-linux/bootlin-toolchain-gcc-93> -O aarch64--glibc--stable-final.tar.gz
tar xf aarch64--glibc--stable-final.tar.gz
cd ../..
wget <https://developer.nvidia.com/downloads/embedded/l4t/r35_release_v4.1/sources/public_sources.tbz2>
tar xjf public_sources.tbz2
cd Linux_for_Tegra/source/public
tar xjf kernel_src.tbz2
cd -

# Run apply_patches_ext_orin.sh from repository root as

./apply_patches_ext_orin.sh Linux_for_Tegra/source/public 5.1.2

# Build by running

./build_all_deb.sh 5.1.2 Linux_for_Tegra/source/public

# Install header deb package under newly created images folder

Keep a backup of the original extlinux configuration which is in /boot/extlinux/extlinux.conf

Edit /boot/extlinux/extlinux.conf primary boot option's LINUX/FDT lines to use built kernel image and dtb file:

LINUX /boot/Image-5.10.104-d457
FDT /usr/lib/linux-image-5.10.104-d457/tegra194-p2888-0001-p2822-0000.dtb

# Make D457 I2C module autoload at boot time

echo "d4xx" | sudo tee /etc/modules-load.d/d4xx.conf

# Note for display drivers

For some reason when building a custom kernel, display drivers are not built in. Keep that in mind in case display output is required
