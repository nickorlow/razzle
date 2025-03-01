#pragma once

#include "serial.h"
#include "../../../arch/i386/asm/asm.h"

#define COM1 (0x3F8)

int serial_init() {
  outb(COM1 + 1, 0x01);
  outb(COM1 + 3, 0x80);
  outb(COM1 + 0, 0x03);
  outb(COM1 + 1, 0x00);
  outb(COM1 + 3, 0x03);
  outb(COM1 + 2, 0xC7);
  outb(COM1 + 4, 0x0B);
  outb(COM1 + 4, 0x0F);
  return 0;
}

int serial_has_tx() { return inb(COM1 + 5) & 0x20; }

int serial_has_rx() { return inb(COM1 + 5) & 1; }

void serial_tx(char a) {
  while (serial_has_tx() == 0)
    ;

  outb(COM1, a);
}

char serial_rx() {
  while (serial_has_rx() == 0)
    ;

  return inb(COM1);
}
