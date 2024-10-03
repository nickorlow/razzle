#pragma once

#include <stdint.h>

void memset(char *ptr, char data, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    ptr[i] = data;
  }
}

void memcpy(char *src, char *dst, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
    dst[i] = src[i];
  }
}

void strncpy(char *src, char *dst, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
    dst[i] = src[i];
    if (src[i] == '\0') {
      return;
    }
  }
}
