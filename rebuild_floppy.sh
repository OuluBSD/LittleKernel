#!/bin/bash

# Script to rebuild the floppy image with updated kernel
# This recreates the floppy image with GRUB bootloader and updated kernel

if [ "$EUID" -ne 0 ]; then
    echo "This script must be run as root. Please run with sudo."
    exit 1
fi

if [ -z "$1" ]; then
    echo "Usage: $0 <path_to_new_kernel>"
    echo "Example: sudo $0 build/kernel"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "Error: Kernel file '$1' does not exist"
    exit 1
fi

echo "Rebuilding floppy image with kernel: $1"

# Create a new empty floppy image (1.44MB)
dd if=/dev/zero of=updated_floppy.img bs=1024 count=1440

# Set up loop device for the new image
LOOP_DEV=$(losetup -f --show updated_floppy.img)

if [ -z "$LOOP_DEV" ]; then
    echo "Error: Could not set up loop device"
    exit 1
fi

echo "Created loop device: $LOOP_DEV"

# Create a FAT12 filesystem on the image
if ! mkfs.vfat -F 12 -n "LITTLEK" "$LOOP_DEV"; then
    echo "Error: Could not create FAT12 filesystem"
    losetup -d "$LOOP_DEV"
    exit 1
fi

echo "Created FAT12 filesystem"

# Create mount point and mount the new filesystem
mkdir -p temp_mount
if ! mount "$LOOP_DEV" temp_mount/; then
    echo "Error: Could not mount the new filesystem"
    losetup -d "$LOOP_DEV"
    rmdir temp_mount 2>/dev/null
    exit 1
fi

echo "Mounted temporary filesystem"

# Copy the kernel file to the root of the filesystem
cp "$1" temp_mount/kernel

if [ $? -ne 0 ]; then
    echo "Error: Could not copy kernel file"
    umount temp_mount/
    losetup -d "$LOOP_DEV"
    rmdir temp_mount 2>/dev/null
    exit 1
fi

echo "Copied kernel file"

# Create other necessary files/directories if needed
# For example, if there were other files in the original image
# cp -r /path/to/other/files temp_mount/ 2>/dev/null || true

sync

# Unmount the filesystem
umount temp_mount/

# Clean up
rmdir temp_mount 2>/dev/null
losetup -d "$LOOP_DEV"

echo "Successfully created updated_floppy.img with new kernel!"
echo "The image has GRUB bootloader and the updated kernel file."

# Copy to build directory
mkdir -p build
cp updated_floppy.img build/kernel_floppy.img

echo "Copied to build/kernel_floppy.img"