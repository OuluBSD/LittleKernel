#!/bin/bash

# Build:
umk ./upptst,./uppsrc "${1}" ~/.config/u++/theide/CLANG.bm -bsH1 +GUI,USEMALLOC ".bin/${1}"

# Run ide:
echo "Executable compiled: .bin/${1}"
