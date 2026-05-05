.PHONY: buildroot-download buildroot-config buildroot kernel-download toolchain-download kernel-config kernel

all: buildroot kernel

buildroot-download:
	if [ ! -e buildroot ]; then \
	wget --no-check-certificate https://buildroot.org/downloads/buildroot-2023.02.1.tar.gz; \
	tar xvf buildroot-2023.02.1.tar.gz; \
	rm buildroot-2023.02.1.tar.gz; \
	mv buildroot-2023.02.1 buildroot; \
	fi

buildroot-config: buildroot-download
	cp buildroot_config buildroot/.config
	cd buildroot; make olddefconfig
	mkdir -p buildroot/board/qemu/patches;
   
buildroot: buildroot-config
	cd buildroot; make -j`nproc --all`
        
kernel-download:
	if [ ! -e kernel ]; then \
	wget -no-check-certificate https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.2.5.tar.xz; \
	tar xvf linux-6.2.5.tar.xz; \
	rm linux-6.2.5.tar.xz; \
	mv linux-6.2.5 kernel; \
	fi

toolchain-download:
	if [ ! -e gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu ]; then \
	wget --no-check-certificate https://developer.arm.com/-/media/Files/downloads/gnu-a/10.3-2021.07/binrel/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz; \
	tar xvf gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz; \
	rm gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz; \
	fi

kernel-config: kernel-download toolchain-download
	cp kernel_config kernel/.config
	cd kernel; ARCH=arm64 make olddefconfig

kernel: kernel-config
	cd kernel; \
	ARCH=arm64 CROSS_COMPILE=`pwd`/../gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu- make -j`nproc --all`

run: buildroot kernel
	qemu-system-aarch64 -kernel kernel/arch/arm64/boot/Image \
	-drive format=raw,file=buildroot/output/images/rootfs.ext4,if=virtio \
	-append "root=/dev/vda console=ttyAMA0 nokaslr" \
	-nographic -M virt -cpu cortex-a72 -m 2G -smp 2
