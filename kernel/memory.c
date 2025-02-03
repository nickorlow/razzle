#pragma once

#include <stdint.h>

// Below doesn't work!
// uint8_t streq(char *ptr, char* ptr2) {
//    while (1 == 1) {
//        if (*ptr != *ptr2) {
//            return 0;
//        }
//
//        if (*ptr == '\0') {
//            return 1;
//        }
//
//        ptr++;
//        ptr2++;
//    }
//}

uint8_t memseq(char *ptr, char *ptr2, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    if (ptr[i] != ptr2[i]) {
      return 0;
    }
  }
  return 1;
}

uint8_t streq(char *str, char *str2) {
  for (uint32_t i = 0;; i++) {
    if (str[i] != str2[i]) {
      return 0;
    } else if (str[i] == '\0' && str2[i] == '\0') {
      return 1;
    }
  }
}

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
