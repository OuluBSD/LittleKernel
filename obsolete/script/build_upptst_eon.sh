#!/bin/sh

# Build:
umk ./upptst,./uppsrc "${1}" ~/.config/u++/theide/CLANG.bm -bsH1 +USEMALLOC,AI,DEBUG_VFS,DEBUG_RT ".bin/${1}"

# Run ide:
echo "Executable compiled: .bin/${1}"
