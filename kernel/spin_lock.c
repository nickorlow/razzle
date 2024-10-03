#pragma once

#ifdef ARCH_I386
#include "../arch/i386/asm.c"
#endif

void sl_acquire(uint32_t *lock) { __sl_acquire(lock); }

void sl_release(uint32_t *lock) { __sl_release(lock); }
