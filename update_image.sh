#!/bin/bash

# Check if the kernel path argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <path_to_kernel_executable>"
    echo "Example: $0 ./build/kernel"
    exit 1
fi

# Check if the kernel file exists
if [ ! -f "$1" ]; then
    echo "Error: Kernel file '$1' does not exist"
    exit 1
fi

# Make sure build directory exists
mkdir -p build

# Copy the original floppy image to the build directory
cp floppy.img build/kernel_floppy.img

# Update the image using mtools with the correct file name format
# Use uppercase KERNEL as FAT filesystems are typically case-insensitive but often store in uppercase
echo "Updating floppy image with kernel using mtools..."
mcopy -i build/kernel_floppy.img -D o "$1" ::KERNEL

if [ $? -eq 0 ]; then
    echo "Successfully updated kernel image using mtools!"
else
    echo "Attempting alternative approach..."
    # Try without the -D o flag
    mcopy -i build/kernel_floppy.img "$1" ::KERNEL 2>/dev/null || {
        echo "Error: Failed to update kernel in floppy image"
        exit 1
    }
    echo "Successfully updated kernel image using mtools!"
fi
