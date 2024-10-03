cmake_minimum_required(VERSION 3.10)

project(razzle C ASM)

# Check architecture is passed and is supported
set(ALLOWED_ARCHS "i386")
if(NOT DEFINED ARCH)
    message(FATAL_ERROR "Architecture not set s not defined. Supported architectures are: ${ALLOWED_ARCHS}")
endif()
list(FIND ALLOWED_ARCHS "${ARCH}" ARCH_INDEX)
if(ARCH_INDEX EQUAL -1)
    message(FATAL_ERROR "Unsupported architecture: ${ARCH}. Supported architectures are: ${ALLOWED_ARCHS}.")
endif()
message(STATUS "Building RAZZLE for ${ARCH}.")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-ffreestanding -g -Wall -Wextra -mgeneral-regs-only -DARCH_I386")
set(CMAKE_ASM_FLAGS "-g")
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker.ld)

# Common source files
FILE(GLOB SOURCES kernel/kernel.c)

# Architecture Specific Files
if (ARCH STREQUAL "i386")
    set(CMAKE_C_COMPILER i386-elf-gcc)
    set(CMAKE_ASM_COMPILER i386-elf-as)
    set(LD i386-elf-gcc)
    FILE(GLOB ASM_SOURCES arch/i386/*.s)
    SET(ARCH_FLAG "-DARCH_I386")
endif()

add_executable(kernel.elf ${SOURCES} ${ASM_SOURCES})
set_target_properties(kernel.elf PROPERTIES LINK_FLAGS "-T ${LINKER_SCRIPT} -ffreestanding -O2 -nostdlib -lgcc -g ${ARCH_FLAG}")
add_custom_command(OUTPUT os.iso
    COMMAND cp kernel.elf ../iso/boot/kernel.elf
    COMMAND genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o os.iso ../iso
    DEPENDS kernel.elf
    COMMENT "Generating OS ISO"
)

# Run & Debug targets 
add_custom_target(run COMMAND qemu-system-i386 -boot d -cdrom os.iso -m 512 -machine type=pc-i440fx-3.1 -monitor stdio DEPENDS os.iso)
add_custom_target(debug COMMAND ./debug.sh DEPENDS os.iso)

