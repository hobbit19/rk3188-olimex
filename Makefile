#
# RK3188 Flash ROM and SDCard make file
#
# Copyright (c) 2018 by Empire of Fun
# Written by B. Eschrich aka britus
#

# -----------------------------------------------------------
# Build environment
# -----------------------------------------------------------

# Project root
BASE := $(HOME)/embedded/rk3188/olimex/board

# Source root
SRC := $(BASE)/src

# Target output
OUT := /tmp/build

# Target directories
OUT_DIR_NAND := $(OUT)/nand
OUT_DIR_ROOTFS := $(OUT)/rootfs

# Kernel definition
ARCH := arm
GNU_EABI := arm-linux-gnueabihf-

KERNEL_CONF := eofmc_rk3188_defconfig
#KERNEL_CONF := olimex_rk3188_defconfig

# -----------------------------------------------------------
# Variables / Definitions
# -----------------------------------------------------------

# Linux kernel, initrd and rootfs sources 
SRC_DIR_KERNEL := $(SRC)/kernel
SRC_DIR_INITRD := $(SRC)/initrd

# Boot loader to use
BOOTLOADER := $(BASE)/loader/v2.19/RK3188Loader_V2.19_DDR2_300MHz.bin

# Board definition
BOARD_ID := 007
BOARD_MODEL := rk3188-som
BOARD_MAGIC := 0x5041524B
BOARD_ATAG := 0x60000800
BOARD_MACHINE := 3066
BOARD_CHECK_MASK := 0x80
KERNEL_IMAGE_ADDR := 0x60408000
RECOVERY_KEY := 1,1,0,20,0
MANUFACTURER := EOFMC

FW_VER_MAJOR := 4
FW_VER_MINOR := 2
FW_VER_BUILD := 2

# Kernel command line  console=ttyFIQ0,115200
# console=ttyFIQ0,115200 console=tty0
# console=ttyS3,115200 console=tty0
KERNEL_CONSOLE := console=ttyFIQ0,115200 console=tty0 androidboot.console=ttyFIQ0

# NAND Partitions SIZE@ADDRESS
NAND_DISK_PARTS := 0x00008000@0x00002000(boot),-@0x0000A000(linuxroot)

# Start address initrd
NAND_INITRD_ADDR := 0x62000000,0x00800000

# Output version
BOARD_FW_VERSION := $(FW_VER_MAJOR).$(FW_VER_MINOR).$(FW_VER_BUILD)

# Firmware version for image
IMAGE_FW_VERSION := $(FW_VER_MAJOR) $(FW_VER_MINOR) $(FW_VER_BUILD)

# NAND flash ROM output file
NAND_FIRMWARE := $(OUT)/rk3188_nand_fw_$(BOARD_FW_VERSION).img

# MAC Address: first byte must FA for local ethernet
NAND_MAC_ADDRESS := FA:45:4F:46:4D:43 

# ----------------------------------------------------------------
# Helpers
# ----------------------------------------------------------------

NAND_KERNEL_CMDL := $(KERNEL_CONSOLE)
NAND_KERNEL_CMDL += root=/dev/block/mtd/by-name/linuxroot
NAND_KERNEL_CMDL += init=/sbin/init
NAND_KERNEL_CMDL += rootfstype=ext4 loglevel=7 rootwait ro
NAND_KERNEL_CMDL += mac_addr=$(NAND_MAC_ADDRESS)
NAND_KERNEL_CMDL += cgroup_disable=memory
NAND_KERNEL_CMDL += initrd=$(NAND_INITRD_ADDR)
NAND_KERNEL_CMDL += mtdparts=rk29xxnand:$(NAND_DISK_PARTS)

# this tools located /usr/local/bin 
AFPTOOL := $(BASE)/bin/afptool
MKNANDIMG := $(BASE)/bin/img_maker
MKBOOTIMG := $(BASE)/bin/mkbootimg
RKFLASHTOOL := $(BASE)/bin/rkflashtool
UPGRADETOOL := $(BASE)/bin/Linux_Upgrade_Tool_v1.2/linux/upgrade_tool

# RootFS default configs and real firmware rootfs
ROOTFS_IMAGE_DIR := $(BASE)/images/rootfs/

# NAND Root FS base image archive
#ROOTFS_IMAGE_ARCHIVE := $(BASE)/images/rootfs-nand-20170927.tar.gz
ROOTFS_IMAGE_ARCHIVE := $(BASE)/images/rootfs-nand-20180111.tar.gz

SRC_ROOTFS_DEFS := $(SRC)/default/rootfs

# ETC config files that may be modified to track changes
ROOTFS_SRC_FILES := $(SRC_ROOTFS_DEFS)/etc/lightdm/lightdm.conf
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/environment
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/hostname
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/hosts
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/inittab
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/logrotate.conf
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/profile
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/rc.local
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/rsyslog.conf
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/resolv.conf
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/sysctl.conf
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/nsswitch.conf
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/sudoers
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/passwd
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/group
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/gshadow
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/shadow
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/issue
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/issue.net
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/motd
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/pointercal
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/ts.conf
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/samba/smb.conf
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/ssh/ssh_config
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/ssh/sshd_config
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/etc/X11/default-display-manager

# X11 config
ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/usr/share/X11/xorg.conf.d/20-touchscreen.conf
#ROOTFS_SRC_FILES += $(SRC_ROOTFS_DEFS)/usr/share/X11/xorg.conf.d/11-display-lcd7.conf

# Determine entire rootfs size
ROOT_FS_SIZE := $$(sudo du -ks $(BASE)/images/rootfs | cut -f 1)

# Kernel build: make options
KRNL_MAKE_OPT := ARCH=$(ARCH) CROSS_COMPILE=$(GNU_EABI) LOCALVERSION="+"

# The kernel file to generate boot.img
_kernel_output := $(SRC_DIR_KERNEL)/arch/arm/boot/Image

# ----------------------------------------------------------------
# Make targets
# ----------------------------------------------------------------

# Default make target
all: init rom

# Cleanup target output
clean:
	@echo "++ Clean build target directory $(OUT)"
	@$(MAKE) kclean
	sudo $(RM) -R $(OUT_DIR_NAND)
	sudo $(RM) -R $(OUT_DIR_ROOTFS)
	sudo $(RM) -R $(OUT)/*.img
	sudo $(RM) $(_kernel_output)
	sudo $(RM) -R $(SRC_ROOTFS_DEFS)/lib/modules

init:
	@echo "++ Prepare target output directory $(OUT)"
	@sudo $(RM) -R $(OUT_DIR_ROOTFS)
	@sudo $(RM) -R $(OUT_DIR_NAND)
	@sudo mkdir -p $(SRC_ROOTFS_DEFS)/lib/modules
	@sudo mkdir -p $(OUT_DIR_ROOTFS)
	@sudo mkdir -p $(OUT_DIR_NAND)

# Build NAND firmware ROM image only
rom: init $(NAND_FIRMWARE)
	
# Show kernel menu config
kconfig:
	@cd $(SRC_DIR_KERNEL) && $(MAKE) -j1 $(KRNL_MAKE_OPT) menuconfig

# Make kernel cleanup
kclean:
	@cd $(SRC_DIR_KERNEL) && $(MAKE) -j4 $(KRNL_MAKE_OPT) clean

# Make kernel modules
kmodules:
	@cd $(SRC_DIR_KERNEL) && $(MAKE) -j4 $(KRNL_MAKE_OPT) modules

# Install kernel modules
kinstall: kmodules
	@cd $(SRC_DIR_KERNEL) && sudo $(MAKE) -j1 $(KRNL_MAKE_OPT) INSTALL_MOD_PATH=$(SRC_ROOTFS_DEFS) modules_install

# Reset kernel config to default
kdefcfg:
	@echo "++ KERNEL: Default config to: $(KERNEL_CONF)"
	@cd $(SRC_DIR_KERNEL) && $(MAKE) -j1 $(KRNL_MAKE_OPT) $(KERNEL_CONF)

# Kernel build only
kernel: 
	@touch $(SRC_DIR_KERNEL)/.config && $(MAKE) $(_kernel_output)

# Make boot image
boot:
	@$(RM) $(OUT_DIR_NAND)/boot.img
	@$(RM) $(OUT_DIR_ROOTFS)/initrd.img
	@$(MAKE) $(OUT_DIR_NAND)/boot.img

# Make rootfs image
rootfs: $(OUT_DIR_NAND)/rootfs.img

# Update flash rom parameter file
parameter: 
	@$(RM) $(OUT_DIR_NAND)/parameter
	@$(MAKE) $(OUT_DIR_NAND)/parameter

# Update boot partition
rkftboot: boot $(OUT_DIR_NAND)/boot.img
	@echo "++ NAND...: Flashing $? to boot parition"
	@$(RKFLASHTOOL) w boot < $(OUT_DIR_NAND)/boot.img

# Update rootfs partition
rkftrootfs: $(OUT_DIR_NAND)/rootfs.img
	@echo "++ NAND...: Flashing $? to linuxroot parition"
	@$(RKFLASHTOOL) w linuxroot < $(OUT_DIR_NAND)/rootfs.img

# Write flash rom image to NAND
flash: rom
	@echo "++ NAND...: Flashing $(NAND_FIRMWARE)"
	@$(UPGRADETOOL) uf $(NAND_FIRMWARE)

# TAKE CARE!
nandllf:
	@echo "++ NAND...: Low level format - NAND is empty!"
	@$(UPGRADETOOL) lf

# Write flash rom parameter file to NAND
flashparam:
	@$(MAKE) parameter
	@echo "++ NAND...: Flash parameter file"
	@$(UPGRADETOOL) di -p $(OUT_DIR_NAND)/parameter

# Write boot loader binary to NAND
flashloader:
	@$(UPGRADETOOL) ul $(BOOTLOADER)

# Get NAND information
flashinfo:
	@echo "++ NAND...: Show flash rom information:"
	@$(RKFLASHTOOL) v
	@$(RKFLASHTOOL) n
	 
# Execute board reset
resetboard:
	@echo "++ Reboot board"
	@$(RKFLASHTOOL) b

# ----------------------------------------------------------------
# Kernel, initrd and boot image file
# ----------------------------------------------------------------

$(_kernel_output): $(SRC_DIR_KERNEL)/.config
	@echo "++ BOOTIMG: Create RK3188 kernel"
	@sudo $(RM) -R $(SRC_ROOTFS_DEFS)/lib/modules
	@sudo $(RM) $(OUT_DIR_NAND)/boot.img
	@cd $(SRC_DIR_KERNEL) && $(MAKE) $(KRNL_MAKE_OPT) kernel.img
	@$(MAKE) kinstall

# ----------------------------------------------------------------
# Boot image file (initrd)
# ----------------------------------------------------------------

olimex_initrd := $(BASE)/images/initrd/initrd.img
$(OUT_DIR_NAND)/boot.img: $(olimex_initrd) $(_kernel_output)
	@echo "++ BOOTIMG: Create boot image file $@"
	@echo "++ BOOTIMG: Kernel file $(_kernel_output)"
	@echo "++ BOOTIMG: RAM Disk file $(olimex_initrd)"
	@$(MKBOOTIMG) --kernel $(_kernel_output) --ramdisk $(olimex_initrd) -o $@

# ----------------------------------------------------------------
# Root file system
# ----------------------------------------------------------------

$(OUT_DIR_NAND)/rootfs.img: $(ROOTFS_SRC_FILES) $(_kernel_output)
	@echo "++ ROOTFS.: Create root fs image file $@"
	@echo "++ ROOTFS.: Image source $(ROOTFS_IMAGE_ARCHIVE)"
	@echo "++ ROOTFS.: Update source $(SRC_ROOTFS_DEFS)"
	@sudo $(BASE)/bin/make-rootfs.sh $(BASE) $(OUT_DIR_ROOTFS) $(SRC_ROOTFS_DEFS) $@ $(ROOTFS_IMAGE_ARCHIVE)
	@sudo umount --force $(OUT_DIR_ROOTFS)
	@sudo rmdir $(OUT_DIR_ROOTFS)
	@sudo chown root:root $@

# ----------------------------------------------------------------
# NAND flash ROM
# ----------------------------------------------------------------
	
$(OUT_DIR_NAND)/recover-script: init
	@echo "++ NAND...: Copy recovery script: $@"
	@touch $@   

$(OUT_DIR_NAND)/update-script: init
	@echo "++ NAND...: Copy update script: $@"
	@touch $@   

$(OUT_DIR_NAND)/bootloader.bin: $(BOOTLOADER)
	@echo "++ NAND...: Copy flash rom bootloader: $? $@"
	@cp -up $? $@   

$(OUT_DIR_NAND)/parameter: init
	@echo "++ NAND...: Write flash rom parameter file: $@"
	@echo "FIRMWARE_VER:$(BOARD_FW_VERSION)" > $@
	@echo "MACHINE_MODEL:$(BOARD_MODEL)" >> $@
	@echo "MACHINE_ID:$(BOARD_ID)" >> $@
	@echo "MANUFACTURER:$(MANUFACTURER)" >> $@
	@echo "MAGIC:$(BOARD_MAGIC)" >> $@
	@echo "ATAG:$(BOARD_ATAG)" >> $@
	@echo "MACHINE:$(BOARD_MACHINE)" >> $@
	@echo "CHECK_MASK:$(BOARD_CHECK_MASK)" >> $@
	@echo "KERNEL_IMG:$(KERNEL_IMAGE_ADDR)" >> $@
	@echo "#RECOVER_KEY:$(RECOVERY_KEY)" >> $@
	@echo "CMDLINE:$(NAND_KERNEL_CMDL)" >> $@

$(OUT_DIR_NAND)/package-file: init
	@echo "++ NAND...: Write flash rom package file: $@"
	@echo "#NAME           Relative path" > $@
	@echo "#HWDEF          HWDEF" >> $@
	@echo "package-file    package-file" >> $@
	@echo "bootloader      bootloader.bin" >> $@
	@echo "parameter       parameter" >> $@
	@echo "boot            boot.img" >> $@
	@echo "linuxroot       rootfs.img" >> $@
	@echo "backup          RESERVED" >> $@
	@echo "update-script   update-script" >> $@
	@echo "recover-script  recover-script" >> $@

# list of all NAND output files
_nand_files := $(OUT_DIR_NAND)/update-script
_nand_files += $(OUT_DIR_NAND)/recover-script
_nand_files += $(OUT_DIR_NAND)/parameter
_nand_files += $(OUT_DIR_NAND)/package-file
_nand_files += $(OUT_DIR_NAND)/bootloader.bin
_nand_files += $(OUT_DIR_NAND)/boot.img
_nand_files += $(OUT_DIR_NAND)/rootfs.img

# NAND flash rom file
raw_temp := $(OUT)/rawimage.img 
$(NAND_FIRMWARE): $(_nand_files)
	@echo "++ NAND...: Create flash rom image: $@"
	@$(AFPTOOL) -pack $(OUT_DIR_NAND) $(raw_temp)
	@$(MKNANDIMG) -rk31 $(BOOTLOADER) $(IMAGE_FW_VERSION) $(raw_temp) $(NAND_FIRMWARE)
	@$(RM) $(raw_temp)

# -----------------------------------------------------------
 
_phony_targets := clean
_phony_targets += init
_phony_targets += kdefcfg
_phony_targets += kconfig
_phony_targets += kmodules
_phony_targets += kernel
_phony_targets += boot
_phony_targets += initrd
_phony_targets += parameter
_phony_targets += nandllf
_phony_targets += flash
_phony_targets += flashinfo
_phony_targets += flashloader
_phony_targets += resetboard
_phony_targets += rootfs

.PHONY: $(_phony_targets)
