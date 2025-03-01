#pragma once

#include "./acpi/rsdp.h"
#include "./drivers/serial/serial.h"
#include "./drivers/vga/vga.h"
#include "./io/io.h"
#include "./pci/pci.h"
#include "./scheduler/scheduler.h"
#include <stdint.h>

#include "../arch/i386/scheduler/scheduler.c"
#include "../supp/test_processes.c"
#include "./drivers/blk/ramblk/ramblk.h"
#include "./drivers/fs/nqfs/nqfs.h"
#include "./drivers/ps2/ps2.h"
#include "drivers/blk/blk.h"

#ifdef ARCH_I386
#include "../arch/i386/init.c"
#endif

int kernel_main() {
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);
  serial_init();
  find_rsdp();
  pci_enumerate_devices();
  VGA_clear_screen();
  interrupt_disable();
  arch_init();
  init_process_table();
  init_ps2();

  // Init block device list
  BLK__init();

  // Init block device drivers
  RAMBLK__init();

  struct block_dev blk_devices[128];
  uint32_t blkdev_cnt = BLK__get_block_devices(blk_devices, 128);

  for(uint32_t i = 0; i < blkdev_cnt; ++i) {
      printf("blk: %d: %s with capacity %d Bytes\n", blk_devices[i].device_no, blk_devices[i].name, (blk_devices[i].num_sectors * blk_devices[i].sector_size));
  }

  start_process("razzle", (char *)&test_razzle);
  start_process("dazzle", (char *)&test_dazzle);

  interrupt_enable();

  for (;;) {
  }
}

void fault_handler(struct regs *r) {
  VGA_clear_screen();
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
