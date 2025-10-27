#!/bin/bash
#
# FreeDOS Floppy Image Download and Mount Script
#

# Configuration
FLOPPY_IMAGE_URL="https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.4/FD14-FloppyEdition.zip"
TEMP_DIR="/tmp/freedos_floppy"
IMAGE_DIR="/home/sblo/Dev/LittleKernel/floppy_images"
ZIP_FILE="$TEMP_DIR/freedos.zip"
EXTRACTED_DIR="$TEMP_DIR/extracted"

# Create directories
mkdir -p "$TEMP_DIR"
mkdir -p "$IMAGE_DIR"

echo "Downloading FreeDOS 1.4 Floppy Edition..."
wget -O "$ZIP_FILE" "$FLOPPY_IMAGE_URL"

if [ $? -ne 0 ]; then
    echo "Failed to download FreeDOS image"
    exit 1
fi

echo "Extracting FreeDOS floppy images..."
unzip -o "$ZIP_FILE" -d "$EXTRACTED_DIR"

if [ $? -ne 0 ]; then
    echo "Failed to extract FreeDOS images"
    exit 1
fi

echo "Copying 1.44MB floppy images to $IMAGE_DIR..."
# Copy all 1.44MB images (ending in .img or .IMA)
find "$EXTRACTED_DIR" -name "*144m*.img" -exec cp {} "$IMAGE_DIR/" \;
find "$EXTRACTED_DIR" -name "*144m*.IMA" -exec cp {} "$IMAGE_DIR/" \;

echo "FreeDOS floppy images downloaded and copied:"
ls -la "$IMAGE_DIR"

echo "Cleaning up temporary files..."
rm -rf "$TEMP_DIR"

echo "Done! FreeDOS floppy images are ready in $IMAGE_DIR"
echo "You can now use these images with QEMU's -fda parameter"