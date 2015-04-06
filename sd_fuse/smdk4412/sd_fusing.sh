#
# Copyright (C) 2011 Samsung Electronics Co., Ltd.
#              http://www.samsung.com/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
####################################

if [ -z $1 ]
then
    echo "usage: ./sd_fusing.sh <SD Reader's device file>"
    exit 0
fi

if [ -b $1 ]
then
    echo "$1 reader is identified."
else
    echo "$1 is NOT identified."
    exit 0
fi

####################################
#<verify device>

BDEV_NAME=`basename $1`
BDEV_SIZE=`cat /sys/block/${BDEV_NAME}/size`

if [ ${BDEV_SIZE} -le 0 ]; then
	echo "Error: NO media found in card reader."
	exit 1
fi

if [ ${BDEV_SIZE} -gt 32000000 ]; then
	echo "Error: Block device size (${BDEV_SIZE}) is too large"
	exit 1
fi

####################################
# check files

E4412_UBOOT=../../u-boot.bin
MKBL2=../mkbl2

if [ ! -f ${E4412_UBOOT} ]; then
	echo "Error: u-boot.bin NOT found, please build it & try again."
	exit -1
fi

if [ ! -f ${MKBL2} ]; then
	echo "Error: can not find host tool - mkbl2, stop."
	exit -1
fi

#<make bl2>
#${MKBL2} ${E4412_UBOOT} bl2.bin 14336

####################################
# fusing images

signed_bl1_position=1
bl2_position=31
uboot_position=63
tzsw_position=719

echo "erase flash"
dd if=/dev/zero of=$1 count=4000

if [ 1 -eq 1 ];then
#<BL1 fusing>
echo "---------------------------------------"
echo "BL1 fusing"
#dd iflag=dsync oflag=dsync if=../../bl1.bin of=$1 seek=$signed_bl1_position
dd iflag=dsync oflag=dsync if=../../bl1.bin of=$1 seek=$signed_bl1_position
fi

if [ 1 -eq 1 ];then
#<BL2 fusing>
echo "---------------------------------------"
echo "BL2 fusing"
dd iflag=dsync oflag=dsync if=../../bl2.bin of=$1 seek=$bl2_position
fi

if [ 1 -eq 1 ];then
echo "---------------------------------------"
echo "u-boot fusing"
echo "dd iflag=dsync oflag=dsync if=${E4412_UBOOT} of=$1 seek=$uboot_position"
dd iflag=dsync oflag=dsync if=${E4412_UBOOT} of=$1 seek=$uboot_position
fi

if [ 1 -eq 1 ];then
#<TrustZone S/W fusing>
echo "---------------------------------------"
echo "TrustZone S/W fusing"
dd iflag=dsync oflag=dsync if=../../tzsw.bin of=$1 seek=$tzsw_position
fi

#<flush to disk>
sync

####################################
#<Message Display>
echo "---------------------------------------"
echo "U-boot image is fused successfully."
echo "Eject SD card and insert it again."

