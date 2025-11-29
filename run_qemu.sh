#!/bin/bash

qemu-system-i386 -fda build/kernel_floppy.img -m 256 "$@"
