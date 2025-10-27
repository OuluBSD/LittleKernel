# Floppy Disk Support in LittleKernel OS

## Overview

LittleKernel OS now includes support for floppy disk images through QEMU's `-fda` parameter. This allows running DOS programs and FreeDOS directly from floppy disk images within the kernel environment.

## Features

1. **QEMU fda Support**: Reads floppy disk images passed via QEMU's `-fda` parameter
2. **1.44MB Floppy Support**: Supports standard 1.44MB 3.5" floppy disk images
3. **Read/Write Operations**: Full read and write support to floppy disk images
4. **FreeDOS Compatibility**: Tested with FreeDOS 1.4 floppy images
5. **Automatic Detection**: Automatically detects and mounts floppy images

## Supported Formats

- 1.44MB 3.5" floppy disk images (.img, .IMA)
- FAT12 file system (standard for floppy disks)
- Compatible with FreeDOS, MS-DOS, and other DOS variants

## Usage Instructions

### 1. Download FreeDOS Floppy Images

Run the download script to get FreeDOS 1.4 floppy images:

```bash
cd /home/sblo/Dev/LittleKernel
./scripts/download_freedos_floppy.sh
```

This will download and extract FreeDOS 1.4 floppy images to the `floppy_images` directory.

### 2. Build the Kernel

Build the LittleKernel OS with the new floppy support:

```bash
cd /home/sblo/Dev/LittleKernel
./build.sh
```

### 3. Run with Floppy Support

Use the provided script to run the kernel with QEMU and a floppy image:

```bash
cd /home/sblo/Dev/LittleKernel
./run_with_floppy.sh
```

This script will automatically use the first available floppy image in the `floppy_images` directory.

Alternatively, you can run QEMU manually with a specific floppy image:

```bash
cd /home/sblo/Dev/LittleKernel
qemu-system-i386 -kernel build/kernel -fda floppy_images/FD14FLOP.IMG -m 128M -serial stdio
```

### 4. Using Custom Floppy Images

To use your own floppy disk images:

1. Place your .img or .IMA files in the `floppy_images` directory
2. Modify the `run_with_floppy.sh` script to use your specific image
3. Or run QEMU manually with your image:

```bash
qemu-system-i386 -kernel build/kernel -fda path/to/your/image.img -m 128M -serial stdio
```

## Technical Details

### Floppy Driver Implementation

The floppy driver (`FloppyDriver`) provides:

- Block device abstraction for floppy disk access
- Automatic detection of floppy disk images
- Support for both read-only and read-write operations
- CHS (Cylinder/Head/Sector) to LBA (Logical Block Address) conversion
- QEMU-specific mode for direct image file access

### System Call Interface

The DOS-KPIv2 (System Call Interface) provides:

- Modern syscall instruction-based interface for DOS programs
- Full compatibility with DOS system calls
- Efficient context switching between kernel and DOS programs
- Support for both interrupt-based (DOS-KPIv1) and syscall-based (DOS-KPIv2) DOS programs

### Memory Management

- 4KB page alignment for efficient memory access
- Virtual memory support through paging
- Shared memory regions for inter-process communication
- Proper memory protection between processes

### File System Support

- FAT12 support for floppy disk images
- Path translation between DOS and Unix-style paths
- Drive letter mapping (A:, C:, etc.)
- Standard file operations (open, read, write, close, etc.)

## Testing

The kernel includes built-in tests for the floppy driver:

1. Boot sector reading and validation
2. Multi-sector read operations
3. Write operations (when not in read-only mode)
4. Bounds checking for sector access
5. Data integrity verification

## Troubleshooting

### Common Issues

1. **Floppy image not found**: Run the download script first
2. **QEMU not found**: Install QEMU with `sudo apt-get install qemu-system-x86`
3. **Kernel build errors**: Ensure all dependencies are installed
4. **Floppy not mounting**: Check that the image file is valid and accessible

### Debugging

Enable verbose logging by setting appropriate log levels in the kernel configuration to troubleshoot floppy driver issues.

## Future Enhancements

Planned improvements include:

1. Hardware floppy controller support for real hardware
2. Support for other floppy formats (720KB, 360KB, etc.)
3. Improved error handling and recovery
4. Enhanced performance optimizations
5. Better integration with the DOS-KPIv2 system call interface

## License

This floppy disk support is part of the LittleKernel OS project and is licensed under the same terms as the rest of the kernel.