#!/bin/sh

# Build:
umk ./upptst,./uppsrc Eon00 ~/.config/u++/theide/CLANG.bm -bsH1 +AI,DEBUG_RT,DEBUG_VFS,USEMALLOC .bin/Eon00

# Run ide:
echo Executable compiled: .bin/Eon00
