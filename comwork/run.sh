#!/bin/sh


qemu-system-aarch64 \
-kernel linux/arch/arm64/boot/Image \
-drive format=raw,file=buildroot/output/images/rootfs.ext4,if=virtio \
-append "root=/dev/vda console=ttyAMA0 nokaslr" \
-nographic -M virt -cpu cortex-a72 \
-m 2G \
-smp 2
