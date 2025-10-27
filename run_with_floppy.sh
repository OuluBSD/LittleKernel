#!/bin/bash
#
# QEMU startup script with FreeDOS floppy support
#

# Configuration
KERNEL_BIN="/home/sblo/Dev/LittleKernel/build/kernel"
FLOPPY_IMAGES_DIR="/home/sblo/Dev/LittleKernel/floppy_images"
DEFAULT_FLOPPY_IMAGE="FD14FLOP.IMG"  # Default FreeDOS 1.44MB floppy image

# Check if kernel binary exists
if [ ! -f "$KERNEL_BIN" ]; then
    echo "Kernel binary not found: $KERNEL_BIN"
    echo "Please build the kernel first using ./build.sh"
    exit 1
fi

# Check if floppy images directory exists
if [ ! -d "$FLOPPY_IMAGES_DIR" ]; then
    echo "Floppy images directory not found: $FLOPPY_IMAGES_DIR"
    echo "Please run the download_freedos_floppy.sh script first"
    exit 1
fi

# Set the floppy image to use
FLOPPY_IMAGE="$FLOPPY_IMAGES_DIR/$DEFAULT_FLOPPY_IMAGE"

# Check if the specific floppy image exists, if not try to find another one
if [ ! -f "$FLOPPY_IMAGE" ]; then
    echo "Default floppy image not found: $FLOPPY_IMAGE"
    # Try to find any .img or .IMA file in the directory
    FIRST_IMG=$(ls "$FLOPPY_IMAGES_DIR"/*.img 2>/dev/null | head -1)
    FIRST_IMA=$(ls "$FLOPPY_IMAGES_DIR"/*.IMA 2>/dev/null | head -1)
    
    if [ -n "$FIRST_IMG" ]; then
        FLOPPY_IMAGE="$FIRST_IMG"
        echo "Using first available .img file: $FLOPPY_IMAGE"
    elif [ -n "$FIRST_IMA" ]; then
        FLOPPY_IMAGE="$FIRST_IMA"
        echo "Using first available .IMA file: $FLOPPY_IMAGE"
    else
        echo "No floppy images found in $FLOPPY_IMAGES_DIR"
        echo "Please run the download_freedos_floppy.sh script first"
        exit 1
    fi
fi

echo "Starting QEMU with kernel: $KERNEL_BIN"
echo "Using floppy image: $FLOPPY_IMAGE"

# Start QEMU with the kernel and floppy image
qemu-system-i386 \
    -kernel "$KERNEL_BIN" \
    -fda "$FLOPPY_IMAGE" \
    -m 128M \
    -serial stdio \
    -display sdl \
    -enable-kvm || \
qemu-system-i386 \
    -kernel "$KERNEL_BIN" \
    -fda "$FLOPPY_IMAGE" \
    -m 128M \
    -serial stdio \
    -display sdl

echo "QEMU exited"