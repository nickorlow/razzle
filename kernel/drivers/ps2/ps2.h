#pragma once

#include <stdint.h>

#include "../../scheduler/scheduler.h"

struct ps2_waiter_t {
  uint8_t  alloc;
  uint32_t pid;
};

extern struct ps2_waiter_t ps2_waiters[MAX_PROCESSES];

void init_ps2();
void handle_keypress_char(char);
void handle_keypress(uint8_t);
