ARCH_x86_64_CC=clang
ARCH_x86_64_TRIPLET=x86_64-none-elf

TARGET_TRIPLET=$(ARCH_x86_64_TRIPLET)
TARGET_LDSCRIPT=linker.ld

TARGET_MAKE=INCLUDES="$(addprefix $(BUILD_DIR)/, $(INCLUDES))" CUR_DIR="$(BUILD_DIR)/$(TARGET_DIR)/$(TARGET_ARCH)" all

TARGET_ARCH_CC=$(ARCH_x86_64_CC)
# Default to the LLVM toolchain (clang as the freestanding assembler driver,
# lld as the linker) so a build needs no separately-built x86_64-elf-gcc cross
# toolchain. Override AS/TARGET_ARCH_LD on the command line to use a GNU cross
# toolchain instead.
TARGET_ARCH_LD ?= clang -target $(ARCH_x86_64_TRIPLET) -fuse-ld=lld
# Unconditional assignment: AS is a GNU Make built-in (defaults to `as`), so
# `?=` would not override it. A command-line `make AS=...` still takes priority.
AS = clang -target $(ARCH_x86_64_TRIPLET)
ASFLAGS+=-ffreestanding -march=x86-64 -m64

EXEC_ENTRY_POINT=0x400000

include $(TARGET_DIR)/$(TARGET_ARCH)/Sources.inc