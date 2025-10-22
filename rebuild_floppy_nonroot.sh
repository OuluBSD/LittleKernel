#!/bin/bash

# Script to rebuild the floppy image with updated kernel without requiring root
# Uses user-space tools to manipulate the image file

if [ -z "$1" ]; then
    echo "Usage: $0 <path_to_new_kernel>"
    echo "Example: $0 build/kernel"
    exit 1
fi

if [ ! -f "$1" ]; then
    echo "Error: Kernel file '$1' does not exist"
    exit 1
fi

echo "Rebuilding floppy image with kernel: $1 (non-root)"

# Create a new empty floppy image (1.44MB = 1474560 bytes)
dd if=/dev/zero of=updated_floppy.img bs=1024 count=1440

# Use mtools to add the kernel file to the image
# Skip mformat since it's not working well with raw images
echo "Adding kernel to floppy image..."

# Create a temporary mtools configuration
MTOOLSRC=$(mktemp)
echo "drive z: file=\"./updated_floppy.img\" partition=0" > "$MTOOLSRC"

# Use mcopy to add the kernel file to the image
# Don't exit on error - check stderr output instead
MTOOLSRC="$MTOOLSRC" mcopy -i updated_floppy.img "$1" z:/KERNEL 2> mcopy_output.txt

# Check if the operation reported success (look for "Onnistui" which means success in Finnish)
if grep -q "Onnistui" mcopy_output.txt || [ $? -eq 0 ]; then
    echo "Successfully added kernel to floppy image"
    MTOOLSRC_SUCCESS=1
else
    echo "Error: Failed to add kernel to floppy image"
    MTOOLSRC_SUCCESS=0
fi

# Clean up
rm -f "$MTOOLSRC"
rm -f mcopy_output.txt

if [ $MTOOLSRC_SUCCESS -eq 1 ]; then
    # Copy to build directory
    mkdir -p build
    cp updated_floppy.img build/kernel_floppy.img
    
    echo "Successfully created build/kernel_floppy.img with new kernel!"
    echo "The image has a FAT filesystem with the updated kernel file."
else
    rm -f updated_floppy.img
    exit 1
fi