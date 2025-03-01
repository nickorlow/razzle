#pragma once

#include <inttypes.h>

void     RAMBLK__init();
uint32_t RAMBLK__read_sector(uint32_t device, uint32_t sector, char *out);
uint32_t RAMBLK__write_sector(uint32_t device, uint32_t sector, char *in);
