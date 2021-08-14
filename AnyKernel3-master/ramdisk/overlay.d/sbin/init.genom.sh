#!/system/bin/sh

# ZRAM Setup
BDEV=/dev/block/platform/bootdevice/by-name/cust
realpath $BDEV > /sys/block/zram0/backing_dev
echo 4294967296 > /sys/block/zram0/disksize
mkswap /dev/block/zram0
swapon /dev/block/zram0
