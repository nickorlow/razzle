#pragma once

#include "../../blk/blk.h"
#include <stdint.h>

#define NQFS_MAX_FILENAME_SIZE (64)
#define NQFS_STORAGE_BYTES_SECTOR (504)

struct NQFS_inode {
  uint8_t  allocd;
  char     filename[NQFS_MAX_FILENAME_SIZE];
  uint32_t start_sector;
  uint32_t size;
  char     _[55];
} __attribute__((packed));

struct NQFS_storage_block {
  uint32_t magic_number;
  char     data[NQFS_STORAGE_BYTES_SECTOR];
  uint32_t next_sector;
} __attribute__((packed));

uint32_t NQFS_get_next_free_inode(struct block_dev *device);
uint32_t NQFS_create_file(struct block_dev *device, char *filename);

// Returns number of bytes read into char
uint32_t NQFS_read_bytes(struct block_dev *device, uint32_t inode_sector,
                         char *out, uint32_t max_size);
uint32_t NQFS_write_bytes(struct block_dev *device, uint32_t inode_sector,
                          char *in, uint32_t size);

void NQFS_DEBUG_list_files(struct block_dev *device);
