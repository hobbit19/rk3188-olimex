#!/bin/bash

#
# Create the default distro rootfs
#

#
# change as needed!
#
BASESRCDIR="${HOME}/embedded/rk3188/src"

# our template sources
DEFSRCDIR="${BASESRCDIR}/default/rootfs"

# default target rootfs
ROOTFSDIR="${BASESRCDIR}/rootfs"

#
# Bind host vfs to rootfs
#
function mount_vfs()
{
	sudo mkdir -p ${ROOTFSDIR}/dev && \
		sudo mount --bind /dev ${ROOTFSDIR}/dev || return 1
	
	sudo mkdir -p ${ROOTFSDIR}/proc && \
		sudo mount --bind /proc ${ROOTFSDIR}/proc || return 1
		
	sudo mkdir -p ${ROOTFSDIR}/sys && \
		sudo mount --bind /sys ${ROOTFSDIR}/sys || return 1
}

#
# Unbind host vfs to rootfs
#
function unmount_vfs()
{
	sudo umount ${ROOTFSDIR}/dev || return 1
	sudo umount ${ROOTFSDIR}/proc || return 1
	sudo umount ${ROOTFSDIR}/sys || return 1
}

#
# Build rootfs based on Ubuntu distro
#
function create_ubuntu()
{
	distro=trusty
	targetdir="${ROOTFSDIR}"
	
	echo "++ Run to first stage..."
		
	# Install the needed packages: 
	sudo apt-get update || return 1
	sudo apt-get install qemu-user-static debootstrap binfmt-support ubuntu-archive-keyring || return 1
		
	# Build first stage:
	mkdir ${targetdir} || return 1
	
	# Run first stage	
	sudo debootstrap \
		--arch=armhf \
		--keyring=/usr/share/keyrings/ubuntu-archive-keyring.gpg \
		--foreign ${distro} ${targetdir} || return 1 

	# Copy needed files from host to the target: 
	sudo cp -vup /usr/bin/qemu-arm-static ${targetdir}/usr/bin/ || return 1
	sudo cp -vup /etc/resolv.conf $DEFSRCDIR/etc/ || return 1
	sudo cp -vup /etc/resolv.conf ${targetdir}/etc/ || return 1

	echo "++ Run to second stage..."
		
	# Login into the new filesystem: 
	sudo chroot ${targetdir} <<EOF
distro=trusty
export LANG=C
/debootstrap/debootstrap --second-stage
exit
EOF

	# Once the package installation has finished, setup some support files and apt configuration.
	sudo sed -e "s/%DISTRO%/$distro/g" ${BASESRCDIR}/default/helper/debian-apt-sources.list > /tmp/xx || return 1
	sudo cp /tmp/xx ${ROOTFSDIR}/etc/apt/sources.list || return 1
}

#
# Build rootfs based on Debian distro
#
function create_debian()
{
	distro=wheezy
	targetdir="${ROOTFSDIR}"

	echo "++ Run to first stage..."
		
	# Install the needed packages: 
	sudo apt-get update || return 1
	sudo apt-get install qemu-user-static debootstrap binfmt-support || return 1
		
	# Build first stage:
	mkdir ${targetdir} || return 1
	
	# Run first stage	
	sudo debootstrap --arch=armhf --foreign ${distro} ${targetdir} || return 1 
	
	# Copy needed files from host to the target:
	sudo cp -vup /usr/bin/qemu-arm-static ${targetdir}/usr/bin/ || return 1
	sudo cp -vup /etc/resolv.conf ${DEFSRCDIR}/etc/ || return 1
	sudo cp -vup /etc/resolv.conf ${targetdir}/etc/ || return 1

	echo "++ Run to second stage..."
		
	# Login into the new filesystem: 
	sudo chroot ${targetdir} <<EOF
distro=wheezy
export LANG=C
/debootstrap/debootstrap --second-stage
exit
EOF

	# Once the package installation has finished, setup some support files and apt configuration.
	sudo sed -e "s/%DISTRO%/$distro/g" ${BASESRCDIR}/default/helper/debian-apt-sources.list > /tmp/xx || return 1
	sudo cp /tmp/xx ${ROOTFSDIR}/etc/apt/sources.list || return 1
}

#
# Finalize installation
# 	Add for X installation: lightdm xfce4 tslib libts-bin tsconf
#
function create_thirdstage()
{
	tempdir="${BASESRCDIR}/default/temp"
	targetdir="${ROOTFSDIR}"
	
	echo "++ Run to third stage..."

	# Bind host vfs to rootfs
	mount_vfs || return 1
								
	# Login into the new filesystem and install additional packages 
	sudo chroot ${targetdir} <<EOF
export LANG=C
apt-get update
apt-get -y install debconf locales dialog sudo less usbutils makedev
apt-get -y install openssh-server ntpdate mc vim ssh kmod module-init-tools
apt-get -y install wpasupplicant wireless-tools htop libkmod2
apt-get -y install lightdm xfce4 tslib libts-bin tsconf
apt-get -y install plymouth xserver-xephyr libpam-gnome-keyring
apt-get -y install xvfb linuxvnc
dpkg-reconfigure locales
chmod u+s /usr/bin/sudo
chown -R man /var/cache/man
useradd -s /bin/bash -m -d /home/eofmc -c "Default User" eofmc
usermod -a -G dialout,adm,sudo,video,audio eofmc
passwd eofmc <<EOF
eofmc
eofmc
> EOF
passwd root <<EOF
eofmc
eofmc
> EOF
apt-get -y autoremove
apt-get -y purge
exit
EOF
}

#
# Update distro config files with our 
# customizations
#
function update_configs()
{
	tempdir="${BASESRCDIR}/default/temp"
	targetdir="${ROOTFSDIR}"
	
	echo "++ Finalize rootfs..."
		
	mkdir -p ${tempdir} || return 1
	
	# fix ownership to root	 
	sudo cp -pr ${DEFSRCDIR}/* ${tempdir}/ || return 1
	sudo chown -R root:root ${tempdir}/* || return 1
	
	# copy customized files
	sudo cp -vupr ${tempdir}/* ${targetdir}/ || return 1
	
	# remove temp directories
	sudo rm -fR ${tempdir} || return 1

	# fix permissions and ownership
	sudo chmod u+s ${targetdir}/usr/bin/sudo
	sudo chown -R man ${targetdir}/var/cache/man
	sudo chown root:root /etc/sudoers.d
		
	# reorder Xorg config
	test -r ${targetdir}/usr/share/X11/xorg.conf.d/10-evdev.conf && \
		cd ${targetdir}/usr/share/X11/xorg.conf.d && \
		sudo mv 10-evdev.conf 99-evdev.conf && cd -
}

#
# Exit on error
#
function show_error_and_exit()
{
	echo "Build error!"
	exit 1
}

##################################################
# Main
##################################################
	
opt="$1"
case ${opt} in
	debian|Debian|DEBIAN)
		create_debian
	;;
	ubuntu|Ubuntu|UBUNTU)
		create_ubuntu
	;;	
	*)
		echo "Usage: $0 debian | ubuntu"
		exit 1
	;;
esac

if [ $? -ne 0 ] ; then
	show_error_and_exit
fi

create_thirdstage
if [ $? -ne 0 ] ; then
	show_error_and_exit
fi

# remove dummy services
plst=`ps -ef | grep arm-static`
for i in $plst ; do
	pid=`echo $i | awk '{print $2}'`
	echo "Kill process $pid"
	kill -9 $pid
done

# unmount vfs from rootfs before update_configs!
unmount_vfs
if [ $? -ne 0 ] ; then
	show_error_and_exit
fi

# Update configs and permissions
update_configs
if [ $? -ne 0 ] ; then
	show_error_and_exit
fi

echo "*********************************************"
echo "FINISH :: ${ROOTFSDIR} successfully built
echo "*********************************************"
