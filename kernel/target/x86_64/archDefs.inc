ARCH_x86_64_CC=clang
ARCH_x86_64_TRIPLET=x86_64-none-elf

TARGET_TRIPLET=$(ARCH_x86_64_TRIPLET)
TARGET_LDSCRIPT=linker.ld

TARGET_MAKE=INCLUDES="$(addprefix $(BUILD_DIR)/, $(INCLUDES))" CUR_DIR="$(BUILD_DIR)/$(TARGET_DIR)/$(TARGET_ARCH)" all

TARGET_ARCH_CC=$(ARCH_x86_64_CC)
TARGET_ARCH_LD=x86_64-elf-gcc
AS=x86_64-elf-gcc 
ASFLAGS+=-ffreestanding -march=x86_64 -m64

EXEC_ENTRY_POINT=0x400000

include $(TARGET_DIR)/$(TARGET_ARCH)/Sources.inc