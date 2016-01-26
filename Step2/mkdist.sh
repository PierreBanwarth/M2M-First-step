#!/bin/sh
# This shell script creates a bootable Qemu disk, in a file.
# First, it creates a disk image (raw) and sets up GRUB.
# Then, it tweaks with the MBR to create a partition
# and makes an ext2 file system on it.
# Finally, it mounts that partition and copies your mini
# distribution.
#
# This script will show you how to do the following:
#  - use dd
#  - use fdisk & parted
#  - control a program through a script (redirection)
#  - use losetup to mount a file as a disk partition
#    or use mount with -o loop option.

GRUB_DIR=./grub/grub
DIST_DIR=./MiniDist
MOUNT_DIR=/mnt/hello
DISK=minidist.img

# First of all, do compile the hello program that will be used
# as the init process when booting just a bare kernel, without 
# any distribution:

(cd hello ; make -s )
cp hello/hello MiniDist/
cp hello/initrd.hello MiniDist/boot/initrd.hello

# Create the raw image...
# with the GRUB stage1 and stage2 at the beginning.
# and some room after to later create a file system

dd if=$GRUB_DIR/lib/grub/i386-pc/stage1 of=$DISK bs=512 count=1
dd if=$GRUB_DIR/lib/grub/i386-pc/stage2 of=$DISK bs=512 seek=1

# we want to create a partition now, starting at sector 256,
# using 128,000 sectors, which is about 64MB.

dd if=/dev/zero of=$DISK bs=512 count=128000 seek=256

# but the disk label in the GRUB boot sector is not recognized 
# by parted... so let's use fdisk to cleanup things a bit first.
# Use fdisk to delete all partitions and rewrite the partition table,
# which seems to cleanup the disk label for parted to work.
fdisk $DISK <<EOF
d 
4
d
3
d
2
d
w
EOF

# now switch to using parted to create the partition,
# one reason is to show you how to use parted, which is
# designed to use from a command line (must easier to script).
# but also because we can control much better the start of 
# partition with parted, which is crucial for the rest of this script.
# Note that we start the partition at sector 256.
parted -s $DISK mkpart primary ext2 256s 128000s

# now we need to have a block device for that partition.
# notice that we have been using the file $DISK as a block
# device, so far. But now we need to use the partition 
# we just created as a device.
# Fortunately, there is support for this in linux:
# through loop devices:
# Note: 256*512=131072
# Could be written
#     sudo losetup -o$((256*512)) /dev/loop1 $DISK

sudo mkdir -p $MOUNT_DIR
sudo losetup -o131072 /dev/loop1 $DISK
sudo mke2fs /dev/loop1


# once the file system has been created, we can mount it,
# there are two ways to do it in Linux.
# The preferred way is to use the loop device we just created:
# mkdir -p $MOUNT_DIR
sudo mount /dev/loop1 $MOUNT_DIR

# It is possible however to mount a file at an offset directly
# with the command mount. We created the partition at sector 256,
# so the offset is 256*512=131072 (512 being the sector size).
#
#    mount -o loop,offset=131072 -t ext2 $DISK $MOUNT_DIR

# Now copy the mini distribution onto the partition we just mounted.
# Notice the use of the archive mode of cp, which is essential 
# in order to keep the file structure (preserves links, ownership,
# and access rights):

( cd $DIST_DIR ; sudo cp -ar . $MOUNT_DIR )

# Sync the file system buffers with the underlying devices:
sync

# Unmount our partition and free the loop device we created
sudo umount /dev/loop1
sudo losetup -d /dev/loop1

echo "Booting qemu on created disk!"
echo "From the GRUB prompt, please install GRUB and reboot:"
echo "  grub> root (hd0,0)"
echo "  grub> setup (hd0)"
echo "  grub> halt"
echo 
qemu-system-i386 -hda $DISK 

echo "Re-booting qemu on created disk,"
echo "you should see your GRUB menu now:"
echo
qemu-system-i386 -hda $DISK -serial stdio




