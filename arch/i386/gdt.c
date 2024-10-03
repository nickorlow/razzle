#pragma once

#include "../../kernel/memory.c"
#include <stdint.h>

struct gdt_row_t {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t granularity_limit_high;
  uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr_t {
  uint16_t size;
  uint32_t address;
} __attribute__((packed));

struct tss_row_t {
  uint32_t prev_tss;
  uint32_t esp_ring0;
  uint32_t ss_ring0;
  uint32_t padding[23]; // Extra options RAZZLE doesn't use
} __attribute__((packed));

enum gdt_access_t {
  GDT_ACCESS_PRESENT = 0b10000000,
  GDT_ACCESS_DPL_KERNEL = 0b00000000,
  GDT_ACCESS_DPL_USER = 0b01100000,
  GDT_ACCESS_CODEDATA = 0b00010000,
  GDT_ACCESS_EXECUTABLE = 0b00001000,
  GDT_ACCESS_GROW_DOWN = 0b00000100,
  GDT_ACCESS_RW = 0b00000010,
  GDT_ACCESS_ACCESSED = 0b00000001,
};

enum gdt_granularity_t {
  GDT_GRANULARITY_4K = 0b10001111,
  GDT_GRANULARITY_MODE_32BIT = 0b01001111,
};

struct tss_row_t tss_entry;

#define GDT_SIZE (6)
struct gdt_row_t gdt[GDT_SIZE];

void load_gdt(struct gdt_ptr_t *gp) {
  asm volatile("lgdt (%0)\n\t"
               "mov $0x10, %%ax\n\t"
               "mov %%ax, %%ds\n\t"
               "mov %%ax, %%es\n\t"
               "mov %%ax, %%fs\n\t"
               "mov %%ax, %%gs\n\t"
               "mov %%ax, %%ss\n\t"
               :
               : "r"(gp)
               : "%ax");
}

void set_gdt_row(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
                 uint8_t granularity) {
  gdt[num].base_low = base & 0xFFFF;
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;
  gdt[num].limit_low = limit & 0xFFFF;
  gdt[num].granularity_limit_high = (limit >> 16) & 0x0F;
  gdt[num].granularity_limit_high |= granularity & 0xF0;
  gdt[num].access = access;
}

void load_tss(uint16_t gdt_row_num) {
  gdt_row_num *= 8;
  __asm__ volatile("mov %0, %%ax; ltr %%ax" : : "r"(gdt_row_num) : "ax");
}

#define KERNEL_CODE_SEL (0x08)
#define KERNEL_DATA_SEL (0x10)

void init_gdt() {
  struct gdt_ptr_t gp;
  gp.size = (sizeof(struct gdt_row_t) * GDT_SIZE) - 1;
  gp.address = (uint32_t)&gdt;

  set_gdt_row(0, 0, 0, 0, 0);

  // Kernelmode code/data degments
  set_gdt_row(1, 0, 0xFFFFFFFF,
              GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_KERNEL | GDT_ACCESS_CODEDATA |
                  GDT_ACCESS_EXECUTABLE | GDT_ACCESS_RW | GDT_ACCESS_ACCESSED,
              GDT_GRANULARITY_4K | GDT_GRANULARITY_MODE_32BIT);
  set_gdt_row(2, 0, 0xFFFFFFFF,
              GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_KERNEL | GDT_ACCESS_CODEDATA |
                  GDT_ACCESS_RW | GDT_ACCESS_ACCESSED,
              GDT_GRANULARITY_4K | GDT_GRANULARITY_MODE_32BIT);

  // Usermode code/data segments
  set_gdt_row(3, 0x0000000, 0xFFFFFFFF,
              GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_USER | GDT_ACCESS_CODEDATA |
                  GDT_ACCESS_EXECUTABLE | GDT_ACCESS_RW | GDT_ACCESS_ACCESSED,
              GDT_GRANULARITY_4K | GDT_GRANULARITY_MODE_32BIT);
  set_gdt_row(4, 0x0000000, 0xFFFFFFFF,
              GDT_ACCESS_PRESENT | GDT_ACCESS_DPL_USER | GDT_ACCESS_CODEDATA |
                  GDT_ACCESS_RW | GDT_ACCESS_ACCESSED,
              GDT_GRANULARITY_4K | GDT_GRANULARITY_MODE_32BIT);

  // TSS row
  set_gdt_row(5, (uint32_t)&tss_entry, sizeof(tss_entry),
              GDT_ACCESS_PRESENT | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_ACCESSED,
              GDT_GRANULARITY_4K | GDT_GRANULARITY_MODE_32BIT);

  // Setup TSS
  int kstack = 0;
  memset((char *)&tss_entry, 0, sizeof(tss_entry));
  tss_entry.ss_ring0 = 0x10;
  tss_entry.esp_ring0 = (uint32_t)&kstack;

  // Load GDT and TSS
  load_gdt(&gp);
  load_tss(5);
}
