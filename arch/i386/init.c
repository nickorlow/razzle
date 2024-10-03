#pragma once 

#include "./gdt.c"
#include "./idt.c"
#include "./irq.c"

void arch_init() {
  init_gdt();
  init_idt();
  init_irq();
}
