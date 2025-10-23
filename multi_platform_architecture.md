# Multi-Platform Architecture Support

This document outlines the multi-platform architecture support for LittleKernel, enabling builds for various hardware platforms including:

- Amiga 500+
- 8088-based PC clone
- 286-based Toshiba T3200
- 32-bit PPC based G4 PowerMac
- 64-bit AMD64
- 64-bit PPC64 based G5 PowerMac

## Architecture Structure

The kernel uses a modular architecture with architecture-specific code separated into different directories:

```
kernel/Kernel/arch/
├── x86/           # x86 (32-bit) specific code
├── x86_64/        # x86_64 (64-bit) specific code
├── ppc/           # PowerPC (32-bit) specific code
├── ppc64/         # PowerPC (64-bit) specific code
├── m68k/          # Motorola 68000 specific code (for Amiga)
└── arm/           # ARM specific code
```

## Configuration System

The build system uses a Linux-style .config file system with target-specific configuration options:

- `CONFIG_TARGET_X86=y` - Build for x86 PC
- `CONFIG_TARGET_AMIGA_500PLUS=y` - Build for Amiga 500+
- `CONFIG_TARGET_8088_PC_CLONE=y` - Build for 8088-based PC clone
- `CONFIG_TARGET_286_TOSHIBA_T3200=y` - Build for 286-based Toshiba T3200
- `CONFIG_TARGET_PPC_G4=y` - Build for 32-bit PPC G4 PowerMac
- `CONFIG_TARGET_AMD64=y` - Build for 64-bit AMD64
- `CONFIG_TARGET_PPC64_G5=y` - Build for 64-bit PPC64 G5 PowerMac

## Build Process

To build for a specific platform:

1. Configure the target in .config:
   ```bash
   make menuconfig
   ```

2. Build the kernel:
   ```bash
   make            # Builds for selected target
   make x86        # Explicitly build for x86
   make amiga_500plus  # Build for Amiga 500+
   make ppc_g4     # Build for G4 PowerMac
   make ppc64_g5   # Build for G5 PowerMac
   make amd64      # Build for AMD64
   ```

## Key Features of Multi-Platform Support

1. **Modular Design**: Architecture-specific code is isolated in separate directories
2. **Common Interface**: All platforms implement the same high-level interfaces through the Hardware Abstraction Layer (HAL)
3. **Configurable Features**: Platform-specific features can be enabled/disabled via .config
4. **Cross-Platform Tools**: Uses appropriate cross-compilers for each target
5. **Build Automation**: Makefile automatically selects appropriate compiler and flags based on target

## Platform-Specific Considerations

### Amiga 500+
- Uses 68000 processor with 68020-compatible instructions allowed
- Requires Amiga-specific boot sequence and AutoConfig support
- Limited to 512KB or 1MB CHIP RAM depending on configuration

### 8088-based PC Clone
- Limited to 16-bit real mode with extended memory access
- Requires special handling for segmented memory model
- Limited available hardware resources

### 286-based Toshiba T3200
- Uses 16-bit protected mode (if possible)
- Limited to approximately 640KB conventional memory
- Specific chipset handling for Toshiba-specific features

### PPC G4 PowerMac
- Uses 32-bit PowerPC architecture
- Supports OpenFirmware boot
- Modern PowerPC features available

### PPC64 G5 PowerMac
- Uses 64-bit PowerPC architecture
- Supports 64-bit addressing
- G5-specific features and optimizations

### AMD64
- Uses x86_64 architecture with 64-bit addressing
- Extended registers and instruction set
- Compatibility with 32-bit code where appropriate

## Future Expansion

The modular design allows for easy expansion to additional platforms:
- New architecture directories can be added following the existing pattern
- HAL interface provides the common abstraction layer
- Configuration system supports platform-specific options
- Build system automatically incorporates new architectures