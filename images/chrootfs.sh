#!/bin/bash

targetdir=rootfs
distro=trusty
echo hostname="rk3188-rootfs" > hostname.txt
sudo cp -uvp /usr/bin/qemu-arm-static ./$targetdir/usr/bin/
sudo cp -uvp /etc/resolv.conf ./$targetdir/etc
sudo cp -uvp hostname.txt ./$targetdir/etc/hostname
echo "Calling CHROOT to ./$targetdir ..."
sudo chroot ./$targetdir

