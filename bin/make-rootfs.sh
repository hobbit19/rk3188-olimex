#!/bin/bash +x

# -----------------------------------
# Create the system root file system
# -----------------------------------
PROJECT_BASE_DIR="$1"
ROOTFS_MOUNT_DIR="$2"
ROOTFS_SOURCE_DIR="$3"
ROOTFS_TARGET_FILE="$4"
ROOTFS_SOURCE_ARCHIVE="$5"

rootfs_size=`du -B 1K -s ${PROJECT_BASE_DIR}/images/rootfs | cut -f 1`
rootfs_human=`du -sh ${PROJECT_BASE_DIR}/images/rootfs | cut -f 1`
echo "++ ROOTFS.: Current block count ${rootfs_size} â 1 KB (${rootfs_human})"

# Create root fs image file (du -B 1K -s ${BASE}/images/rootfs)
sudo dd status=none if=/dev/zero of=${ROOTFS_TARGET_FILE} bs=2K count=${rootfs_size}
test $? -ne 0 && exit 255

# Create root fs partition
echo "++ ROOTFS.: Create ext4 partition 'linuxroot'"
sudo mkfs.ext4 -q -F -L linuxroot ${ROOTFS_TARGET_FILE}
test $? -ne 0 && exit 255

# Mount root fs partition
echo "++ ROOTFS.: Attempt to mount partition 'linuxroot'"
sudo mkdir -p ${ROOTFS_MOUNT_DIR}
test $? -ne 0 && exit 255

sudo mount -o loop ${ROOTFS_TARGET_FILE} ${ROOTFS_MOUNT_DIR}
test $? -ne 0 && exit 255

# Extract base root file system
echo "++ ROOTFS.: Extract file system archive `basename ${ROOTFS_SOURCE_ARCHIVE}`"
cd ${ROOTFS_MOUNT_DIR} && sudo tar xzpf ${ROOTFS_SOURCE_ARCHIVE}
test $? -ne 0 && exit 255

echo "++ ROOTFS.: Update permissions and configurations"

# Copy modified configuration and library files
sudo cp -pa ${ROOTFS_SOURCE_DIR}/* ${ROOTFS_MOUNT_DIR}/
test $? -ne 0 && exit 255

# Create root home
sudo mkdir -p ${ROOTFS_MOUNT_DIR}/root
test $? -ne 0 && exit 255

# Setup directory permissions of root home
sudo chmod 600 ${ROOTFS_MOUNT_DIR}/root
test $? -ne 0 && exit 255

cp -pR ${PROJECT_BASE_DIR}/../../mod-* ${ROOTFS_MOUNT_DIR}/root
test $? -ne 0 && exit 255

# Update sudoers permissions
sudo chmod 600 ${ROOTFS_MOUNT_DIR}/etc/sudoers
test $? -ne 0 && exit 255
	
# Update su permissions
sudo chmod u+s ${ROOTFS_MOUNT_DIR}/bin/su
test $? -ne 0 && exit 255

sudo chmod u+s ${ROOTFS_MOUNT_DIR}/usr/bin/sudo
test $? -ne 0 && exit 255
	
# Update man directory permissions
sudo chown -R man ${ROOTFS_MOUNT_DIR}/var/cache/man
test $? -ne 0 && exit 255

# Update X11 configuration directory permissions
sudo chown -R root:root ${ROOTFS_MOUNT_DIR}/usr/share/X11
test $? -ne 0 && exit 255

# Update system configuration directory permissions
sudo chown -R root:root ${ROOTFS_MOUNT_DIR}/etc
test $? -ne 0 && exit 255

# Flush changes
sync

# success
echo "++ ROOTFS.: Completed successfully"
exit 0
