#include "./ramblk.h"
#include "../../../memory/memory.h"
#include "../blk.h"

#define RAMFS_BYTES (1024 * 1024 * 5) // 5 MB
#define SECTOR_SIZE (512)

char ramfs_blocks[RAMFS_BYTES];

uint32_t RAMBLK__read_sector(uint32_t device, uint32_t sector, char *out) {
  if (sector * SECTOR_SIZE >= RAMFS_BYTES) {
    // Out of bounds
    return 0;
  }

  uint32_t byte = sector * SECTOR_SIZE;

  memcpy(ramfs_blocks + byte, out, SECTOR_SIZE);
  return 1;
}

uint32_t RAMBLK__write_sector(uint32_t device, uint32_t sector, char *in) {
  if ((sector + 1) * SECTOR_SIZE >= RAMFS_BYTES) {
    // Out of bounds
    return 0;
  }

  uint32_t byte = sector * SECTOR_SIZE;

  memcpy(in, ramfs_blocks + byte, SECTOR_SIZE);
  return 1;
}

void RAMBLK__init() { 
    memset(ramfs_blocks, 0x00, RAMFS_BYTES); 
    struct block_dev bd = {
        .name = "RAM_BLK",
        .sector_size = SECTOR_SIZE,
        .device_no = 0,
        .num_sectors = RAMFS_BYTES / SECTOR_SIZE,
        .read_sector = &RAMBLK__read_sector,
        .write_sector = &RAMBLK__write_sector
    };
    BLK__register_device(&bd);
}
