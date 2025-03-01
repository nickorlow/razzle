#include "io.h"
#include "../drivers/serial/serial.h"
#include "../drivers/vga/vga.h"
#include <stdint.h>

#include "../spin_lock.h"

void putc(char c) {
  VGA_putc(c);
  serial_tx(c);
}

void print_uint(unsigned int x, uint8_t base) {
  if (x == 0) {
    if (base)
      putc('0');
    return;
  }

  print_uint(x / 10, 0);
  putc('0' + (x % 10));
}

void print_int(int x, uint8_t base) {
  if (x == 0) {
    if (base)
      putc('0');
    return;
  }

  if (x < 0) {
    putc('-');
    print_int(-1 * (x / 10), 0);
    putc('0' + (x % 10));
  } else {
    print_int(x / 10, 0);
    putc('0' + (x % 10));
  }
}

void print_hex(uint32_t x) {
  putc('0');
  putc('x');

  uint8_t fp = 1;

  do {
    uint32_t digit = (x & 0xF0000000) >> 28;
    if (digit != 0 || !fp || x == 0) {
      putc((digit < 10) ? (digit + '0') : (digit - 10 + 'A'));
      fp = 0;
    }
    x <<= 4;
  } while (x != 0);
}

void print_string(char *s) {
  for (uint32_t i = 0; s[i] != '\0'; i++) {
    putc(s[i]);
  }
}

uint32_t lock;

void printf_base(char *str, va_list list) {

  for (uint32_t i = 0; str[i] != '\0'; i++) {
    if (str[i] == '%') {
      switch (str[i + 1]) {
      case '\0':
        return;
      case '%': {
        putc('%');
        break;
      }

      case 'c': {
        putc(va_arg(list, int));
        break;
      }

      case 'd': {
        print_int(va_arg(list, int), 1);
        break;
      }

      case 'x': {
        print_hex(va_arg(list, int));
        break;
      }

      case 'u': {
        print_uint(va_arg(list, unsigned int), 1);
        break;
      }

      case 's': {
        print_string(va_arg(list, char *));
        break;
      }
      }
      i++;
    } else {
      putc(str[i]);
    }
  }
  VGA_flush();
}

void printf_nolock(char *str, ...) {
  va_list list;
  va_start(list, str);
  printf_base(str, list);
}

void printf(char *str, ...) {
  va_list list;
  va_start(list, str);
  sl_acquire(&lock);
  printf_base(str, list);
  sl_release(&lock);
}
