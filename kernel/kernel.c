#include "./scheduler.c"
#include "./vga.c"
#include <stdint.h>

#include "./test_processes.c"

#ifdef ARCH_I386
#include "../arch/i386/init.c"
#endif

int kernel_main() {
  lock = 0;

  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);

  clear_screen();

  interrupt_disable();

  arch_init();

  init_process_table();

  start_process("razzle", (char *) &test_razzle);
  start_process("dazzle", (char *) &test_dazzle);

  interrupt_enable();

  for (;;) {
  }
}

void fault_handler(struct regs *r) {
  clear_screen();
  printf("!!! Kernel Panic !!!\n");
  switch (r->int_no) {
  case 0: {
    printf("Error: Divide by Zero\n");
    break;
  };

  case 6: {
    printf("Error: Illegal instruction\n");
    break;
  };

  case 8: {
    printf("Error: Double Fault (Ignoring)\n");
    break;
  };

  case 13: {
    printf("Error: General Protection Fault at\n");
    break;
  };

  default: {
    printf("Error: Unknown (no: %d)\n", r->int_no);
    break;
  };
  }

  printf("EIP: %x\n", r->eip);
  printf("ESPv: %x\n", *(((unsigned int *)r->esp) + 2));
  printf("ESP : %x\n", r->esp);

  printf("\n");

  process_debug();

  for (;;) {
  }
}
