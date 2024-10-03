#pragma once

#include "./spin_lock.c"
#include <stdarg.h>
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

static uint8_t col = 0;
static uint8_t row = 0;
static uint8_t color_code = 10;
static char *vram = (char *)0xb8000;

void clear_screen() {
  for (int i = 0; i < HEIGHT - 1; i++) {
    for (int j = 0; j < WIDTH; j++) {
      vram[(i * WIDTH * 2) + (j * 2)] = 0;
      // vram[(i * WIDTH * 2) + (j*2 + 1)] = 0;
    }
  }
  row = 0;
  col = 0;
}

void putc(char c) {
  if (col == WIDTH) {
    row = (row + 1) % HEIGHT;
    col = 0;
  }

  if (c == '\n') {
    for (int i = col; i < WIDTH; i++) {
      vram[(row * WIDTH * 2) + (i * 2)] = ' ';
      vram[(row * WIDTH * 2) + (i * 2 + 1)] = color_code;
    }
    row = (row + 1) % HEIGHT;
    col = 0;
    return;
  }

  if (c == '\b') {
    col--;
    vram[(row * WIDTH * 2) + (col * 2)] = ' ';
    vram[(row * WIDTH * 2) + (col * 2 + 1)] = color_code;
    return;
  }

  vram[(row * WIDTH * 2) + (col * 2)] = c;
  vram[(row * WIDTH * 2) + (col * 2 + 1)] = color_code;
  col++;
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

void print_hex(int x) {
  putc('0');
  putc('x');

  do {
    unsigned int digit = (x & 0xF0000000) >> 28;
    putc((digit < 10) ? (digit + '0') : (digit - 10 + 'A'));
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
