#!/bin/bash

# Script to dump AST for LittleKernel source files using clang
# This will help create detailed documentation and UML diagrams

echo "Dumping AST for LittleKernel source files..."

# Create directory for AST dumps
mkdir -p ast_dumps

# Define source files to analyze
SOURCE_FILES=(
    "kernel/LittleKernel/Common.h"
    "kernel/LittleKernel/Kernel.h" 
    "kernel/LittleKernel/Global.h"
    "kernel/LittleKernel/Monitor.h"
    "kernel/LittleKernel/Monitor.cpp"
    "kernel/LittleKernel/DescriptorTable.h"
    "kernel/LittleKernel/DescriptorTable.cpp"
    "kernel/LittleKernel/Heap.h"
    "kernel/LittleKernel/Heap.cpp"
    "kernel/LittleKernel/Paging.h"
    "kernel/LittleKernel/Paging.cpp"
    "kernel/LittleKernel/Task.h"
    "kernel/LittleKernel/Task.cpp"
    "kernel/LittleKernel/Timer.h"
    "kernel/LittleKernel/Timer.cpp"
    "kernel/LittleKernel/Interrupts.h"
    "kernel/LittleKernel/Interrupts.cpp"
    "kernel/LittleKernel/FileSystem.h"
    "kernel/LittleKernel/FileSystem.cpp"
    "kernel/LittleKernel/Syscall.h"
    "kernel/LittleKernel/Syscall.cpp"
    "kernel/LittleKernel/OrderedArray.h"
    "kernel/LittleKernel/Initrd.h"
    "kernel/LittleKernel/Initrd.cpp"
    "kernel/LittleLibrary/LittleLibrary.h"
    "kernel/LittleLibrary/Callback.h"
    "kernel/LittleLibrary/FixedArray.h"
    "kernel/LittleLibrary/Memory.h"
    "kernel/LittleLibrary/icxxabi.h"
    "kernel/LittleKernel/main.cpp"
)

# Check if clang is available
if ! command -v clang &> /dev/null; then
    echo "clang not found, trying clang++"
    if ! command -v clang++ &> /dev/null; then
        echo "clang++ also not found. Please install clang."
        exit 1
    fi
    CLANG="clang++"
else
    CLANG="clang"
fi

# Compile flags for kernel compilation
FLAGS="-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -march=i386 -std=c++17 -Ikernel"

# Process each source file
for file in "${SOURCE_FILES[@]}"; do
    if [ -f "$file" ]; then
        filename=$(basename "$file")
        echo "Processing $file..."
        $CLANG $FLAGS -Xclang -ast-dump -fsyntax-only "$file" > "ast_dumps/${filename}.ast" 2>&1
        echo "AST dumped to ast_dumps/${filename}.ast"
    else
        echo "File $file not found, skipping..."
    fi
done

echo "AST dumping completed. Check ast_dumps/ directory."