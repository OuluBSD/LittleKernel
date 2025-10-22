#!/bin/bash

# Script to extract data from the floppy image with sudo privileges
# Run this script as: sudo ./extract_floppy.sh

if [ "$EUID" -ne 0 ]; then
    echo "This script must be run as root. Please run with sudo."
    exit 1
fi

echo "Extracting data from floppy.img..."

# Create directory to store extracted data
mkdir -p extracted_floppy
cd extracted_floppy

# First, let's get the basic information about the image
echo "Getting image information..."
fdisk -l ../floppy.img 2>/dev/null || echo "fdisk not available or no partition table"

# Check the file type
echo "Checking file type..."
file ../floppy.img

# Extract boot sector (first 512 bytes)
echo "Extracting boot sector..."
dd if=../floppy.img of=boot_sector.bin bs=512 count=1

# Mount the image to extract files
echo "Creating temporary mount point..."
mkdir -p temp_mount

# Mount the floppy image
if mount -o loop ../floppy.img temp_mount/; then
    echo "Successfully mounted the floppy image."
    
    # Show directory listing
    echo "Directory listing:"
    ls -la temp_mount/
    
    # Copy all files from the mounted image
    echo "Copying files..."
    cp -r temp_mount/* . 2>/dev/null || echo "No files to copy or copy failed"
    
    # Show contents of copied files
    echo "File contents and types:"
    for file in *; do
        if [ -f "$file" ]; then
            echo "File: $file"
            file "$file"
            ls -la "$file"
            echo "---"
        fi
    done
    
    # Unmount
    umount temp_mount/
else
    echo "Failed to mount the floppy image directly."
    echo "Let's try with different options or investigate manually."
    
    # Check if there's a FAT filesystem by looking for FAT signatures
    echo "Looking for FAT signature in the image..."
    hexdump -C ../floppy.img | head -20
    
    # Try to find kernel file in the raw image
    echo "Searching for 'kernel' string in the raw image..."
    strings ../floppy.img | grep -i kernel | head -10
fi

# Clean up
rmdir temp_mount 2>/dev/null || true

echo "Extraction completed in the 'extracted_floppy' directory."
echo "Files saved:"
ls -la

echo ""
echo "Boot sector hexdump (first 512 bytes):"
hexdump -C boot_sector.bin

cd ..