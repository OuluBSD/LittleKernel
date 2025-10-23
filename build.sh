#!/bin/bash

set -e  # Exit on any error

echo "Building LittleKernel..."

# Check if .config exists, if not create a default one
if [ ! -f ".config" ]; then
    echo "Creating default .config file..."
    cat << 'EOF' > .config
# LittleKernel Configuration

# Enable this to build for x86 architecture
CONFIG_X86=y

# Enable this to build for x86_64 architecture
# CONFIG_X86_64 is not set

# Enable serial console output
CONFIG_SERIAL_CONSOLE=y

# Enable VGA console output
CONFIG_VGA_CONSOLE=y

# Enable kernel debugging features
CONFIG_KERNEL_DEBUG=y

# Enable verbose logging
CONFIG_VERBOSE_LOG=n

# Enable runtime configuration changes
CONFIG_RUNTIME_CONFIG=y

# Enable hardware abstraction layer
CONFIG_HAL=y

# Enable profiling infrastructure
CONFIG_PROFILING=n

# Enable module loading system
CONFIG_MODULES=y

# Enable PCI support
CONFIG_PCI=y

# Enable USB support
# CONFIG_USB is not set

# Enable networking support
# CONFIG_NETWORKING is not set

# Memory management options
CONFIG_MMU=y
CONFIG_PAGING=y

# Process management options
CONFIG_PROCESS_MGMT=y
CONFIG_THREAD_MGMT=n

# File system options
CONFIG_VFS=y
CONFIG_FAT32=y

# Timer configuration
CONFIG_TIMER_HZ=100

# Maximum number of processes
CONFIG_MAX_PROCESSES=128

# Kernel heap size in MB
CONFIG_KERNEL_HEAP_SIZE=16

# Enable early memory management
CONFIG_EARLY_MEM=y

# Enable hardware diagnostics
CONFIG_HW_DIAGNOSTICS=y

# Enable error handling framework
CONFIG_ERROR_HANDLING=y
EOF
    echo "Default .config created"
fi

# Check for required tools (make, python for menuconfig)
if ! command -v make &> /dev/null; then
    echo "Warning: make not found, using umk directly"
fi

if ! command -v python3 &> /dev/null; then
    echo "Warning: python3 not found for menuconfig"
fi

# Generate C++ header from .config
if [ -f "scripts/config_header_generator.py" ]; then
    python3 scripts/config_header_generator.py
elif [ -f "scripts/menuconfig.py" ]; then
    # Run the config parser to generate header
    echo "/* Auto-generated from .config */" > kernel_config_defines.h
    echo "#ifndef _KERNEL_CONFIG_DEFINES_H" >> kernel_config_defines.h
    echo "#define _KERNEL_CONFIG_DEFINES_H" >> kernel_config_defines.h
    grep -E 'CONFIG_.*=' .config | while read -r line; do
        if [[ $line == *=y ]]; then
            config=$(echo $line | cut -d'=' -f1)
            echo "#define $config 1" >> kernel_config_defines.h
        elif [[ $line == *=n ]]; then
            config=$(echo $line | cut -d'=' -f1)
            echo "#define $config 0" >> kernel_config_defines.h
        elif [[ $line == *=* ]]; then
            config=$(echo $line | cut -d'=' -f1)
            value=$(echo $line | cut -d'=' -f2-)
            echo "#define $config $value" >> kernel_config_defines.h
        fi
    done
    echo "#endif /* _KERNEL_CONFIG_DEFINES_H */" >> kernel_config_defines.h
fi

echo "Building kernel with UMKA..."

# Run the build with umk
umk kernel Kernel kernel/Kernel/CLANG_K32.bm -dsvb build/kernel

echo "Build completed successfully!"
echo "Kernel binary: build/kernel"