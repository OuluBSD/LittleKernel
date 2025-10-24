# LittleKernel Makefile
# Integrates with .config system and build.sh script

# Include configuration
-include .config

# Determine target architecture from config
ifeq ($(CONFIG_TARGET_AMIGA_500PLUS), y)
	ARCH := m68k
	CROSS_COMPILE := m68k-elf-
	TARGET_PLATFORM := AMIGA_500PLUS
endif

ifeq ($(CONFIG_TARGET_8088_PC_CLONE), y)
	ARCH := x86
	CROSS_COMPILE := i386-elf-
	TARGET_PLATFORM := 8088_PC_CLONE
	# For 8088, limit to 16-bit compatibility features
endif

ifeq ($(CONFIG_TARGET_286_TOSHIBA_T3200), y)
	ARCH := x86
	CROSS_COMPILE := i386-elf-
	TARGET_PLATFORM := 286_TOSHIBA_T3200
	# For 286, limit to 286 compatibility features
endif

ifeq ($(CONFIG_TARGET_PPC_G4), y)
	ARCH := ppc
	CROSS_COMPILE := powerpc-elf-
	TARGET_PLATFORM := PPC_G4
endif

ifeq ($(CONFIG_TARGET_AMD64), y)
	ARCH := x86_64
	CROSS_COMPILE := x86_64-elf-
	TARGET_PLATFORM := AMD64
endif

ifeq ($(CONFIG_TARGET_PPC64_G5), y)
	ARCH := ppc64
	CROSS_COMPILE := powerpc64-elf-
	TARGET_PLATFORM := PPC64_G5
endif

ifeq ($(CONFIG_TARGET_X86), y)
	ARCH := x86
	CROSS_COMPILE := i386-elf-
	TARGET_PLATFORM := X86
endif

# Default if no target is explicitly set
TARGET_PLATFORM ?= X86
ARCH ?= x86

# Compiler settings
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
AS := $(CROSS_COMPILE)as
LD := $(CROSS_COMPILE)ld

# Configuration variables (with defaults that can be overridden by .config)
CONFIG_X86 ?= n
CONFIG_X86_64 ?= n
CONFIG_PPC ?= n
CONFIG_PPC_64 ?= n
CONFIG_SERIAL_CONSOLE ?= y
CONFIG_VGA_CONSOLE ?= y
CONFIG_AMIGA_CONSOLE ?= n
CONFIG_KERNEL_DEBUG ?= y
CONFIG_VERBOSE_LOG ?= n
CONFIG_RUNTIME_CONFIG ?= y
CONFIG_HAL ?= y
CONFIG_PROFILING ?= n
CONFIG_MODULES ?= y
CONFIG_PCI ?= y
CONFIG_AUTOCONFIG ?= n
CONFIG_USB ?= n
CONFIG_NETWORKING ?= n
CONFIG_MMU ?= y
CONFIG_PAGING ?= y
CONFIG_PROCESS_MGMT ?= y
CONFIG_THREAD_MGMT ?= n
CONFIG_VFS ?= y
CONFIG_FAT32 ?= y
CONFIG_TIMER_HZ ?= 100
CONFIG_MAX_PROCESSES ?= 128
CONFIG_KERNEL_HEAP_SIZE ?= 16
CONFIG_EARLY_MEM ?= y
CONFIG_HW_DIAGNOSTICS ?= y
CONFIG_ERROR_HANDLING ?= y

# Architecture-specific flags
ifeq ($(ARCH), x86)
	CXXFLAGS := -m32 -std=c++17 -ffreestanding -fno-exceptions -fno-rtti -nostdlib -nostartfiles -march=i386
	ASFLAGS := -f elf32
	LDFLAGS := -m elf_i386
endif

ifeq ($(ARCH), x86_64)
	CXXFLAGS := -m64 -std=c++17 -ffreestanding -fno-exceptions -fno-rtti -nostdlib -nostartfiles -march=x86-64
	ASFLAGS := -f elf64
	LDFLAGS := -m elf_x86_64
endif

ifeq ($(ARCH), m68k)
	CXXFLAGS := -std=c++17 -ffreestanding -fno-exceptions -fno-rtti -nostdlib -nostartfiles -m68020
	ASFLAGS := -f elf
	LDFLAGS := -m m68kelf
endif

ifeq ($(ARCH), ppc)
	CXXFLAGS := -std=c++17 -ffreestanding -fno-exceptions -fno-rtti -nostdlib -nostartfiles -mcpu=G4 -m32
	ASFLAGS := -f elf
	LDFLAGS := -m elf32ppc
endif

ifeq ($(ARCH), ppc64)
	CXXFLAGS := -std=c++17 -ffreestanding -fno-exceptions -fno-rtti -nostdlib -nostartfiles -mcpu=G5 -m64
	ASFLAGS := -f elf64
	LDFLAGS := -m elf64ppc
endif

# Directories
KERNEL_DIR := kernel/Kernel
LIB_DIR := kernel/Library
BUILD_DIR := build/$(TARGET_PLATFORM)
OBJ_DIR := $(BUILD_DIR)/obj
ISO_DIR := $(BUILD_DIR)/iso
ISO_BOOT_DIR := $(ISO_DIR)/boot
ISO_GRUB_DIR := $(ISO_BOOT_DIR)/grub

# Source files (common)
COMMON_SOURCES := $(wildcard $(KERNEL_DIR)/*.cpp)
COMMON_ASM_SOURCES := $(wildcard $(KERNEL_DIR)/*.asm)
COMMON_SOURCES += $(wildcard $(KERNEL_DIR)/*/*.cpp) $(wildcard $(KERNEL_DIR)/*/*.asm)

# Architecture-specific sources
X86_SOURCES := $(wildcard $(KERNEL_DIR)/arch/x86/*.cpp) $(wildcard $(KERNEL_DIR)/arch/x86/*.asm)
PPC_SOURCES := $(wildcard $(KERNEL_DIR)/arch/ppc/*.cpp) $(wildcard $(KERNEL_DIR)/arch/ppc/*.asm)
M68K_SOURCES := $(wildcard $(KERNEL_DIR)/arch/m68k/*.cpp) $(wildcard $(KERNEL_DIR)/arch/m68k/*.asm)

# Select sources based on architecture
ifeq ($(ARCH), x86)
	ARCH_SOURCES := $(X86_SOURCES)
endif

ifeq ($(ARCH), x86_64)
	ARCH_SOURCES := $(X86_SOURCES)
endif

ifeq ($(ARCH), ppc)
	ARCH_SOURCES := $(PPC_SOURCES)
endif

ifeq ($(ARCH), ppc64)
	ARCH_SOURCES := $(PPC_SOURCES)
endif

ifeq ($(ARCH), m68k)
	ARCH_SOURCES := $(M68K_SOURCES)
endif

SOURCES := $(COMMON_SOURCES) $(ARCH_SOURCES)
ASM_SOURCES := $(filter %.asm, $(SOURCES))

# Object files
COMMON_OBJECTS := $(addprefix $(OBJ_DIR)/, $(notdir $(filter %.cpp, $(COMMON_SOURCES:.cpp=.o))))
ARCH_OBJECTS := $(addprefix $(OBJ_DIR)/, $(notdir $(filter %.cpp, $(ARCH_SOURCES:.cpp=.o))))
ASM_OBJECTS := $(addprefix $(OBJ_DIR)/, $(notdir $(ASM_SOURCES:.asm=.o)))

OBJECTS := $(COMMON_OBJECTS) $(ARCH_OBJECTS) $(ASM_OBJECTS)

# Build targets
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_ISO := $(BUILD_DIR)/kernel.iso

# Common C++ flags
CXXFLAGS += -Wall -Wextra -Werror

# Add debug flags if enabled
ifeq ($(CONFIG_KERNEL_DEBUG), y)
	CXXFLAGS += -g -DDEBUG
endif

# Add verbose logging if enabled
ifeq ($(CONFIG_VERBOSE_LOG), y)
	CXXFLAGS += -DVERBOSE_LOG
endif

# Add profiling if enabled
ifeq ($(CONFIG_PROFILING), y)
	CXXFLAGS += -DPROFILING_ENABLED
endif

# Add modules if enabled
ifeq ($(CONFIG_MODULES), y)
	CXXFLAGS += -DMODULES_ENABLED
endif

# Add platform-specific flags
ifeq ($(TARGET_PLATFORM), AMIGA_500PLUS)
	CXXFLAGS += -DTARGET_AMIGA_500PLUS
endif

ifeq ($(TARGET_PLATFORM), PPC_G4)
	CXXFLAGS += -DTARGET_PPC_G4
endif

ifeq ($(TARGET_PLATFORM), PPC64_G5)
	CXXFLAGS += -DTARGET_PPC64_G5
endif

# Assembler flags
# Already set based on architecture

# Linker flags - add the link script
LDFLAGS += -T $(KERNEL_DIR)/link.ld -nostdlib -lgcc

# Include directories
INCLUDES := -I. -I$(KERNEL_DIR) -I$(LIB_DIR) -I$(KERNEL_DIR)/arch/$(ARCH)

# Libraries (none for kernel)
LIBS :=

# Default target
.PHONY: all
all: $(KERNEL_ISO)

# Include the configuration header generation rule
$(OBJ_DIR)/config_header.o: .config
	@mkdir -p $(dir $@)
	@echo "/* Auto-generated from .config */" > kernel_config_defines.h
	@echo "#ifndef _KERNEL_CONFIG_DEFINES_H" >> kernel_config_defines.h
	@echo "#define _KERNEL_CONFIG_DEFINES_H" >> kernel_config_defines.h
	@grep -E 'CONFIG_.*=' .config | while read -r line; do \
		if [ -n "$$line" ] && [ "$${line:0:1}" != "#" ]; then \
			config=$$(echo "$$line" | cut -d'=' -f1); \
			value=$$(echo "$$line" | cut -d'=' -f2-); \
			if [ "$$value" = "y" ]; then \
				echo "#ifdef $$config" >> kernel_config_defines.h; \
				echo "#undef $$config" >> kernel_config_defines.h; \
				echo "#endif" >> kernel_config_defines.h; \
				echo "#define $$config 1" >> kernel_config_defines.h; \
			elif [ "$$value" = "n" ]; then \
				echo "#ifdef $$config" >> kernel_config_defines.h; \
				echo "#undef $$config" >> kernel_config_defines.h; \
				echo "#endif" >> kernel_config_defines.h; \
				echo "#define $$config 0" >> kernel_config_defines.h; \
			else \
				echo "#ifdef $$config" >> kernel_config_defines.h; \
				echo "#undef $$config" >> kernel_config_defines.h; \
				echo "#endif" >> kernel_config_defines.h; \
				echo "#define $$config $$value" >> kernel_config_defines.h; \
			fi; \
		fi; \
	done
	@echo "#endif /* _KERNEL_CONFIG_DEFINES_H */" >> kernel_config_defines.h
	@echo "Generated kernel_config_defines.h from .config"

# Build kernel ELF
$(KERNEL_ELF): $(OBJECTS) $(OBJ_DIR)/config_header.o
	@mkdir -p $(dir $@)
	@echo "Linking $@ for $(TARGET_PLATFORM)"
	@$(LD) -o $@ $(LDFLAGS) $(OBJECTS)

# Build ISO image (only for x86 targets)
$(KERNEL_ISO): $(KERNEL_ELF)
	@mkdir -p $(ISO_GRUB_DIR)
	@cp $(KERNEL_ELF) $(ISO_BOOT_DIR)/kernel.bin
	@echo "set timeout=0" > $(ISO_GRUB_DIR)/grub.cfg
	@echo "set default=0" >> $(ISO_GRUB_DIR)/grub.cfg
	@echo "" >> $(ISO_GRUB_DIR)/grub.cfg
	@echo "menuentry \"LittleKernel ($(TARGET_PLATFORM))\" {" >> $(ISO_GRUB_DIR)/grub.cfg
	@echo "    multiboot /boot/kernel.bin" >> $(ISO_GRUB_DIR)/grub.cfg
	@echo "    boot" >> $(ISO_GRUB_DIR)/grub.cfg
	@echo "}" >> $(ISO_GRUB_DIR)/grub.cfg
	@echo "Creating ISO: $@"
	@grub-mkrescue -o $@ $(ISO_DIR) 2>/dev/null || genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o $@ $(ISO_DIR)

# Compile C++ source files
$(OBJ_DIR)/%.o: $(KERNEL_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "CXX $<"
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

$(OBJ_DIR)/%.o: $(KERNEL_DIR)/%/*.cpp
	@mkdir -p $(dir $@)
	@echo "CXX $<"
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "CXX $<"
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

$(OBJ_DIR)/%.o: $(LIB_DIR)/%/*.cpp
	@mkdir -p $(dir $@)
	@echo "CXX $<"
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

# Architecture-specific source compilation
$(OBJ_DIR)/%.o: $(KERNEL_DIR)/arch/$(ARCH)/%.cpp
	@mkdir -p $(dir $@)
	@echo "CXX $< (arch-specific)"
	@$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCLUDES)

# Assemble assembly files
$(OBJ_DIR)/%.o: $(KERNEL_DIR)/%.asm
	@mkdir -p $(dir $@)
	@echo "ASM $<"
	@$(AS) $(ASFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(KERNEL_DIR)/arch/$(ARCH)/%.asm
	@mkdir -p $(dir $@)
	@echo "ASM $< (arch-specific)"
	@$(AS) $(ASFLAGS) $< -o $@

# Platform-specific build targets
.PHONY: x86
x86:
	$(MAKE) TARGET_CONFIG=CONFIG_TARGET_X86=y all

.PHONY: amiga_500plus
amiga_500plus:
	$(MAKE) TARGET_CONFIG=CONFIG_TARGET_AMIGA_500PLUS=y all

.PHONY: 8088_pc_clone
8088_pc_clone:
	$(MAKE) TARGET_CONFIG=CONFIG_TARGET_8088_PC_CLONE=y all

.PHONY: 286_toshiba_t3200
286_toshiba_t3200:
	$(MAKE) TARGET_CONFIG=CONFIG_TARGET_286_TOSHIBA_T3200=y all

.PHONY: ppc_g4
ppc_g4:
	$(MAKE) TARGET_CONFIG=CONFIG_TARGET_PPC_G4=y all

.PHONY: amd64
amd64:
	$(MAKE) TARGET_CONFIG=CONFIG_TARGET_AMD64=y all

.PHONY: ppc64_g5
ppc64_g5:
	$(MAKE) TARGET_CONFIG=CONFIG_TARGET_PPC64_G5=y all

# Menuconfig target
.PHONY: menuconfig
menuconfig:
	@echo "Starting menuconfig..."
	@python3 scripts/menuconfig.py

.PHONY: dialogconfig
dialogconfig:
	@echo "Starting dialog-based configuration..."
	@scripts/configure_kernel.sh

# Clean build files
.PHONY: clean
clean:
	@echo "Cleaning build files..."
	@rm -rf build
	@rm -f kernel_config_defines.h

# Clean all including config
.PHONY: distclean
distclean: clean
	@rm -f .config

# Run target using QEMU (only for x86/x86_64)
.PHONY: run
run: $(KERNEL_ISO)
	@echo "Running kernel in QEMU for $(TARGET_PLATFORM)..."
ifeq ($(filter $(TARGET_PLATFORM), X86 AMD64), $(TARGET_PLATFORM))
	@qemu-system-i386 -cdrom $(KERNEL_ISO) -serial stdio
else ifeq ($(TARGET_PLATFORM), PPC_G4)
	@qemu-system-ppc -cdrom $(KERNEL_ISO) -serial stdio
else ifeq ($(TARGET_PLATFORM), PPC64_G5)
	@qemu-system-ppc64 -cdrom $(KERNEL_ISO) -serial stdio
else ifeq ($(TARGET_PLATFORM), AMIGA_500PLUS)
	@echo "Amiga target requires appropriate emulator, kernel built at $(KERNEL_ELF)"
else
	@echo "Target $(TARGET_PLATFORM) requires specific emulator, kernel built at $(KERNEL_ELF)"
endif

.PHONY: run_debug
run_debug: $(KERNEL_ISO)
	@echo "Running kernel in QEMU with GDB support for $(TARGET_PLATFORM)..."
ifeq ($(filter $(TARGET_PLATFORM), X86 AMD64), $(TARGET_PLATFORM))
	@qemu-system-i386 -s -S -cdrom $(KERNEL_ISO) -serial stdio
else
	@echo "GDB support only available for x86 targets in this configuration, kernel built at $(KERNEL_ELF)"
endif

# Build only kernel without creating ISO
.PHONY: kernel
kernel: $(KERNEL_ELF)

# Help target
.PHONY: help
help:
	@echo "LittleKernel Makefile"
	@echo "===================="
	@echo "Available targets:"
	@echo "  all                 - Build complete kernel ISO (default)"
	@echo "  kernel              - Build only the kernel ELF"
	@echo "  menuconfig          - Run configuration menu"
	@echo "  dialogconfig        - Run dialog-based configuration menu"
	@echo "  clean               - Clean build files"
	@echo "  distclean           - Clean all including config"
	@echo "  run                 - Build and run in appropriate emulator"
	@echo "  run_debug           - Build and run with GDB support (x86 only)"
	@echo "  x86                 - Build for x86 (default PC)"
	@echo "  amiga_500plus       - Build for Amiga 500+"
	@echo "  8088_pc_clone       - Build for 8088-based PC clone"
	@echo "  286_toshiba_t3200   - Build for 286-based Toshiba T3200"
	@echo "  ppc_g4              - Build for 32-bit PPC G4 PowerMac"
	@echo "  amd64               - Build for 64-bit AMD64"
	@echo "  ppc64_g5            - Build for 64-bit PPC64 G5 PowerMac"
	@echo "  help                - Show this help"
	@echo ""
	@echo "Configuration is read from .config file"
	@echo "Use 'make menuconfig' to modify configuration"
	@echo "Use 'make dialogconfig' to run dialog-based configuration"

# Include build.sh integration
.PHONY: build_sh_compat
build_sh_compat:
	@echo "Building with build.sh compatibility..."
	@./build.sh

# Silent build target
.PHONY: silent
silent:
	@$(MAKE) --silent all