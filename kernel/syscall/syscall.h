#pragma once

#include <stdint.h>

#define POWERCTL_shutdown (0)
#define POWERCTL_reboot (1)

void     powerctl(uint32_t);
char     getc();
uint32_t lspci();
uint32_t lsproc();
void     yield();
