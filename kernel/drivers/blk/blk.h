#pragma once

#include <stdint.h>

struct block_dev {
    char name[16];
  uint32_t sector_size;
  uint32_t device_no;
  uint32_t num_sectors;
  uint32_t (*read_sector)(uint32_t, uint32_t, char *);
  uint32_t (*write_sector)(uint32_t, uint32_t, char *);
};

void BLK__init();
uint32_t BLK__register_device(struct block_dev*);
uint32_t BLK__get_block_devices(struct block_dev* devices, uint32_t req_num);
uint32_t BLK__get_block_device(struct block_dev* devices, uint32_t req_num);
