#!/bin/bash

# run_bochs.sh
# runs bochs with the kernel floppy image.

bochs -f kernel/LittleKernel/bochsrc.txt -q
