#!/bin/bash

# Clean the build directory before building
rm -rf build/kernel

umk kernel Kernel kernel/Kernel/CLANG_K32.bm -dsvb build/kernel