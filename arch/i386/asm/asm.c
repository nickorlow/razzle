#include "asm.h"

void __sl_acquire(uint32_t *lock_id) {
  __asm__("retry_lock: lock bts $0,(%0); pause; jc retry_lock" : "+g"(lock_id));
}

void __sl_release(uint32_t *lock_id) {
  __asm__("lock btr $0, (%0)" : "+g"(lock_id));
}
