#pragma once

#include "vga.h"
#include "../../memory/memory.h"
#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

static char vga_buf[HEIGHT * (WIDTH * 2)];

static uint8_t vgabuf_col = 0;
static uint8_t vgabuf_row = 0;

static uint8_t vgabuf_start_row = 0;
static uint8_t vgabuf_move_row = 0;

static uint8_t default_color_code = 10;
static char   *vram = (char *)0xb8000;

static inline int VGA_idx(int row, int col) {
  return (row * WIDTH * 2) + (col * 2);
}

static inline void VGA_set(int row, int col, char c, int color_code) {
  vga_buf[VGA_idx(row, col)] = c;
  vga_buf[VGA_idx(row, col) + 1] = color_code;
}

void VGA_flush() {
  int topwrite = ((HEIGHT - vgabuf_start_row) * WIDTH * 2);

  // Fill the top of the display from the offsetted buffer
  memcpy(vga_buf + (vgabuf_start_row * WIDTH * 2), vram, topwrite);

  // Fill the bottom of the display from the offsetted buffer
  memcpy(vga_buf, vram + topwrite, ((vgabuf_start_row)*WIDTH * 2));
}

void VGA_clear_screen() {
  for (int r = 0; r < HEIGHT - 1; r++) {
    for (int c = 0; c < WIDTH; c++) {
      VGA_set(r, c, ' ', default_color_code);
    }
  }

  vgabuf_start_row = 0;
  vgabuf_col = 0;
  vgabuf_row = 0;

  VGA_flush();
}

void VGA_clear_line() {
  for (int i = 0; i < WIDTH; ++i) {
    VGA_set(vgabuf_start_row, i, ' ', default_color_code);
  }
}

void VGA_putc(char c) {
  if (vgabuf_col == WIDTH) {
    vgabuf_row = (vgabuf_row + 1) % HEIGHT;
    vgabuf_col = 0;

    if (vgabuf_start_row > 0 || vgabuf_row == 0) {
      if (vgabuf_move_row == vgabuf_start_row) {
        vgabuf_start_row = (vgabuf_start_row + 1) % HEIGHT;
        vgabuf_move_row = vgabuf_start_row;
      } else {
        vgabuf_move_row++;
      }
      VGA_clear_line();
    }
  }

  if (c == '\n') {
    for (int i = vgabuf_col; i < WIDTH; i++) {
      VGA_set(vgabuf_row, i, ' ', default_color_code);
    }
    vgabuf_row = (vgabuf_row + 1) % HEIGHT;
    vgabuf_col = 0;

    if (vgabuf_start_row > 0 || vgabuf_row == 0) {
      if (vgabuf_move_row == vgabuf_start_row) {
        vgabuf_start_row = (vgabuf_start_row + 1) % HEIGHT;
        vgabuf_move_row = vgabuf_start_row;
      } else {
        vgabuf_move_row++;
      }
      VGA_clear_line();
    }
    return;
  }

  if (c == '\b') {
    if (vgabuf_col == 0) {
      vgabuf_col = WIDTH - 1;
      vgabuf_row--;
      vgabuf_move_row--;
    } else {
      vgabuf_col--;
    }
    // VGA_set(vgabuf_row, vgabuf_col, ' ', default_color_code);
    return;
  }

  VGA_set(vgabuf_row, vgabuf_col, c, default_color_code);
  vgabuf_col++;
}
