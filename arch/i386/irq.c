#pragma once

#include "../../kernel/ps2.c"
#include "../../kernel/syscall.c"
#include "./asm.c"
#include "./idt.c"
#include "handlers.h"

enum irq_code_t {
  IRQCODE_PIT = 32,
  IRQCODE_PS2 = 33,
  IRQCODE_SYSCALL = 128,
};

void kb_handler() {
  uint8_t char_code = inb(0x60);
  handle_keypress(char_code);
}

void remap_irq(void) {
  outb(0x20, 0x11);
  outb(0xA0, 0x11);
  outb(0x21, 0x20);
  outb(0xA1, 0x28);
  outb(0x21, 0x04);
  outb(0xA1, 0x02);
  outb(0x21, 0x01);
  outb(0xA1, 0x01);
  outb(0x21, 0x0);
  outb(0xA1, 0x0);
}

void init_irq() {
  remap_irq();

  set_idt_row(32, (uint32_t)irq0, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(33, (uint32_t)irq1, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(34, (uint32_t)irq2, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(35, (uint32_t)irq3, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(36, (uint32_t)irq4, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(37, (uint32_t)irq5, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(38, (uint32_t)irq6, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(39, (uint32_t)irq7, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(40, (uint32_t)irq8, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(41, (uint32_t)irq9, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(42, (uint32_t)irq10, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(43, (uint32_t)irq11, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(44, (uint32_t)irq12, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(45, (uint32_t)irq13, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(46, (uint32_t)irq14, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);
  set_idt_row(47, (uint32_t)irq15, KERNEL_CODE_SEL, KERNEL_INTERRUPT_FLAG);

  set_idt_row(128, (uint32_t)irq128, KERNEL_CODE_SEL, USER_TRAP_FLAG);
}

void irq_handler(struct regs *r) {
  switch (r->int_no) {
  case IRQCODE_SYSCALL: {
    syscall(r);
    break;
  };

  case IRQCODE_PIT: {
    schedule(r);
    break;
  };

  case IRQCODE_PS2: {
    kb_handler();
    break;
  };
  }

  // Send EOI to follower controller 
  if (r->int_no >= 40 && r->int_no < 48) {
    outb(0xA0, 0x20);
  }

  // Send EOI to leader controller 
  outb(0x20, 0x20);
}
