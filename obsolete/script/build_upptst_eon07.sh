#!/bin/sh

# Build:
umk ./upptst,./uppsrc Eon07 ~/.config/u++/theide/CLANG.bm -bsH1 +AI,SCREEN,SDL2,OGL,FFMPEG,OPENCV,X11,USEMALLOC,DEBUG_RT .bin/Eon07

# Run ide:
echo Executable compiled: .bin/Eon07
