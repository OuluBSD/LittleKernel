#!/bin/bash

# This script runs the LittleKernel OS with various options
# The 'umk' executable belongs to the Ultimate++ framework
# It's often packaged as 'upp', but often must be manually installed

show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -d, --direct            Run kernel directly with -kernel option (default)"
    echo "  -f, --floppy-user       Update floppy image as user (default behavior)"
    echo "  -s, --sudo              Update floppy image with sudo (requires password)"
    echo "  --serial                Enable serial console connection"
    echo "  --headless              Run in headless mode (no GUI, useful for testing)"
    echo "  -n, --no-timeout        Disable timeout (default: timeout 10 seconds)"
    echo ""
    echo "Examples:"
    echo "  $0                      # Run directly with serial (default)"
    echo "  $0 -f --serial        # Update floppy as user with serial"
    echo "  $0 -s                 # Update floppy with sudo"
    echo "  $0 -d --serial        # Run directly with serial"
    echo "  $0 --headless         # Run in headless mode"
    echo "  $0 --no-timeout       # Run without timeout"
}

# Default options
DIRECT_RUN=1
FLOPPY_USER=0
FLOPPY_SUDO=0
SERIAL_ENABLED=1
HEADLESS_MODE=0
NO_TIMEOUT=0
TIMEOUT_DURATION=10  # Default timeout of 10 seconds

# Parse command line options
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -d|--direct)
            DIRECT_RUN=1
            FLOPPY_USER=0
            FLOPPY_SUDO=0
            shift
            ;;
        -f|--floppy-user)
            DIRECT_RUN=0
            FLOPPY_USER=1
            FLOPPY_SUDO=0
            shift
            ;;
        -s|--sudo)
            DIRECT_RUN=0
            FLOPPY_USER=0
            FLOPPY_SUDO=1
            shift
            ;;
        --serial)
            SERIAL_ENABLED=1
            shift
            ;;
        --headless)
            HEADLESS_MODE=1
            shift
            ;;
        -n|--no-timeout)
            NO_TIMEOUT=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Build the kernel first
echo "Building kernel..."
./build.sh

# Set up QEMU command based on options
QEMU_CMD="qemu-system-i386 -m 256 -no-reboot"

if [ $HEADLESS_MODE -eq 1 ]; then
    QEMU_CMD="$QEMU_CMD -nographic"
    # In headless mode, if serial is enabled, use monitor for serial output
    if [ $SERIAL_ENABLED -eq 1 ]; then
        QEMU_CMD="$QEMU_CMD -serial mon:stdio"
    fi
else
    # Not in headless mode, regular serial output
    if [ $SERIAL_ENABLED -eq 1 ]; then
        QEMU_CMD="$QEMU_CMD -serial stdio"
    fi
fi

if [ $DIRECT_RUN -eq 1 ]; then
    # Default behavior: direct kernel run
    if [ $NO_TIMEOUT -eq 1 ]; then
        echo "Running kernel directly with command: $QEMU_CMD -kernel ./build/kernel (no timeout)"
        $QEMU_CMD -kernel ./build/kernel
    else
        echo "Running kernel directly with command: timeout $TIMEOUT_DURATION $QEMU_CMD -kernel ./build/kernel"
        timeout $TIMEOUT_DURATION $QEMU_CMD -kernel ./build/kernel
    fi
elif [ $FLOPPY_USER -eq 1 ]; then
    # Update floppy as user
    echo "Updating floppy image as user..."
    if [ -f "./rebuild_floppy_nonroot.sh" ]; then
        ./rebuild_floppy_nonroot.sh ./build/kernel
        echo "Running kernel from floppy image..."
        if [ $NO_TIMEOUT -eq 1 ]; then
            $QEMU_CMD -fda build/kernel_floppy.img
        else
            timeout $TIMEOUT_DURATION $QEMU_CMD -fda build/kernel_floppy.img
        fi
    else
        echo "Error: rebuild_floppy_nonroot.sh not found"
        echo "Falling back to direct kernel run..."
        if [ $NO_TIMEOUT -eq 1 ]; then
            $QEMU_CMD -kernel ./build/kernel
        else
            timeout $TIMEOUT_DURATION $QEMU_CMD -kernel ./build/kernel
        fi
    fi
elif [ $FLOPPY_SUDO -eq 1 ]; then
    # Update floppy with sudo
    if command -v sudo >/dev/null 2>&1 && [ -f "./rebuild_floppy.sh" ]; then
        echo "Updating floppy image with sudo..."
        sudo ./rebuild_floppy.sh ./build/kernel
        echo "Running kernel from floppy image..."
        if [ $NO_TIMEOUT -eq 1 ]; then
            $QEMU_CMD -fda build/kernel_floppy.img
        else
            timeout $TIMEOUT_DURATION $QEMU_CMD -fda build/kernel_floppy.img
        fi
    else
        echo "Error: sudo not available or rebuild_floppy.sh not found"
        echo "Falling back to direct kernel run..."
        if [ $NO_TIMEOUT -eq 1 ]; then
            $QEMU_CMD -kernel ./build/kernel
        else
            timeout $TIMEOUT_DURATION $QEMU_CMD -kernel ./build/kernel
        fi
    fi
else
    # Default to user approach if no option specified
    echo "Updating floppy image as user (default)..."
    if [ -f "./rebuild_floppy_nonroot.sh" ]; then
        ./rebuild_floppy_nonroot.sh ./build/kernel
        echo "Running kernel from floppy image..."
        if [ $NO_TIMEOUT -eq 1 ]; then
            $QEMU_CMD -fda build/kernel_floppy.img
        else
            timeout $TIMEOUT_DURATION $QEMU_CMD -fda build/kernel_floppy.img
        fi
    else
        echo "Falling back to direct kernel run..."
        if [ $NO_TIMEOUT -eq 1 ]; then
            $QEMU_CMD -kernel ./build/kernel
        else
            timeout $TIMEOUT_DURATION $QEMU_CMD -kernel ./build/kernel
        fi
    fi
fi