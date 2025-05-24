#!/bin/bash

# compile exploits and kernel module
mkdir -p rootfs/home/nobodyQQ
make -C src
make -C yflkp_src

# copy kernel module to root file system
cp yflkp_src/yflkp.ko rootfs


# build initramfs
cd rootfs
mkdir -pv {bin,sbin,etc,proc,dev,tmp,sys,usr/{bin,sbin}}
find . -print0 | cpio --null -ov --format=newc 2>/dev/null | gzip -9 > ../initramfs.cpio.gz

# run qemu
cd .. #&& exec ./run.sh
