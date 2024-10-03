#pragma once

#include "../../kernel/memory.c"
#include "./handlers.h"
#include "gdt.c"

struct idt_row_t {
  uint16_t base_lo;
  uint16_t sel;
  uint8_t unset;
  uint8_t flags;
  uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr_t {
  uint16_t size;
  uint32_t address;
} __attribute__((packed));

// TODO: double check this?
enum idt_flag_t {
  IDTFLAG_PRESENT = 0b10000000,
  IDTFLAG_DPL_KERNEL = 0b00000000,
  IDTFLAG_DPL_USER = 0b01100000,
  IDTFLAG_TRAP = 0b00001111,
  IDTFLAG_INTERRUPT = 0b00001110,
};

#define KERNEL_INTERRUPT_FLAG (IDTFLAG_PRESENT | IDTFLAG_DPL_KERNEL | IDTFLAG_INTERRUPT)
#define USER_TRAP_FLAG (IDTFLAG_PRESENT | IDTFLAG_DPL_USER | IDTFLAG_INTERRUPT)

#define IDT_SIZE (256)
struct idt_row_t idt[IDT_SIZE];
struct idt_ptr_t idtp;

void load_idt(struct idt_ptr_t *pt) { asm volatile("lidt (%0)" ::"r"(pt)); }

void set_idt_row(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
  idt[num].base_hi = (base >> 16) & 0x0000FFFF;
  idt[num].base_lo = base & 0x0000FFFF;
  idt[num].sel = sel;
  idt[num].flags = flags;
  idt[num].unset = 0;
}

void init_idt() {
  idtp.size = (sizeof(struct idt_row_t) * IDT_SIZE) - 1;
  idtp.address = (uint32_t)&idt;

  memset((char *)&idt, 0, sizeof(struct idt_row_t) * IDT_SIZE);

  set_idt_row(0, (uint32_t)isr0, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(1, (uint32_t)isr1, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(2, (uint32_t)isr2, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(3, (uint32_t)isr3, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(4, (uint32_t)isr4, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(5, (uint32_t)isr5, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(6, (uint32_t)isr6, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(7, (uint32_t)isr7, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(8, (uint32_t)isr8, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(9, (uint32_t)isr9, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(10, (uint32_t)isr10, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(11, (uint32_t)isr11, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(12, (uint32_t)isr12, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(13, (uint32_t)isr13, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(14, (uint32_t)isr14, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(15, (uint32_t)isr15, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(16, (uint32_t)isr16, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(17, (uint32_t)isr17, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(18, (uint32_t)isr18, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(19, (uint32_t)isr19, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(20, (uint32_t)isr20, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(21, (uint32_t)isr21, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(22, (uint32_t)isr22, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(23, (uint32_t)isr23, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(24, (uint32_t)isr24, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(25, (uint32_t)isr25, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(26, (uint32_t)isr26, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(27, (uint32_t)isr27, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(28, (uint32_t)isr28, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(29, (uint32_t)isr29, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(30, (uint32_t)isr30, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(31, (uint32_t)isr31, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);

  load_idt(&idtp);
}
