// Nick's Quick(ly made) File System
#include "nqfs.h"
#include "../../../io/io.h"
#include "../../../memory/memory.h"

#define FILEPTR_SECTORS (128)
#define FILEPTR_PER_SECTOR (4)
#define FILEPTR_CNT (FILEPTR_SECTORS * FILEPTR_PER_SECTOR)

void NQFS_init() {}

uint32_t NQFS_get_next_free_block(struct block_dev *device) {
  char                       sector[512];
  struct NQFS_storage_block *sb = (struct NQFS_storage_block *)sector;

  for (uint32_t i = FILEPTR_SECTORS; i < device->num_sectors; ++i) {
    if (!device->read_sector(device->device_no, i, sector)) {
      return 0;
    }
    if (sb->magic_number != 0xC001) {
      sb->magic_number = 0xC001;
      sb->next_sector = 0;
      if (!device->write_sector(device->device_no, i, sector)) {
        return 0;
      }
      return i;
    }
  }

  return 0;
}

uint32_t NQFS_create_file(struct block_dev *device, char *filename) {
  char sector[512];
  for (int i = 0; i < FILEPTR_SECTORS; ++i) {
    if (!device->read_sector(device->device_no, i, sector)) {
      return 0;
    }
    struct NQFS_inode *fp = (struct NQFS_inode *)sector;
    for (int j = 0; j < FILEPTR_PER_SECTOR; ++j) {
      if (!fp->allocd) {
        uint32_t new_sector = NQFS_get_next_free_block(device);
        if (new_sector == 0) {
          return 0;
        }
        fp->allocd = 1;
        fp->size = 0;
        fp->start_sector = new_sector;
        strncpy(filename, fp->filename, NQFS_MAX_FILENAME_SIZE);
        fp->filename[NQFS_MAX_FILENAME_SIZE - 1] = '\0';
        if (!device->write_sector(device->device_no, i, sector)) {
          return 0;
        }
        printf_nolock("Created file \"%s\" at sector  %d fp %d\n", fp->filename,
                      new_sector, i);
        return i;
      }
      fp++;
    }
  }
  return 0;
}
uint32_t NQFS_read_bytes(struct block_dev *device, uint32_t inode_sector,
                         char *out, uint32_t size) {
  char               sector[512];
  struct NQFS_inode *fp = (struct NQFS_inode *)sector;
  if (!device->read_sector(device->device_no, inode_sector, sector)) {
    return 0;
  }
  char                       storage_sector[512];
  struct NQFS_storage_block *sb = (struct NQFS_storage_block *)storage_sector;

  if (fp->start_sector != 0) {
    uint32_t read_sector = fp->start_sector;

    if (size > fp->size) {
      size = fp->size;
    }

    while (size > 0) {
      if (!device->read_sector(device->device_no, read_sector,
                               storage_sector)) {
        return 0;
      }

      uint32_t readable = NQFS_STORAGE_BYTES_SECTOR;
      if (size < readable) {
        readable = size;
      }

      // Copy data into struct
      memcpy(sb->data, out, readable);
      out += readable;
      size -= readable;

      if (size > 0) {
        read_sector = sb->next_sector;
        if (read_sector == 0) {
          return size;
        }
      }
    }

    return size;

  } else {
    printf_nolock("didn't find start\n");
    return 0;
  }
}

uint32_t NQFS_write_bytes(struct block_dev *device, uint32_t inode_sector,
                          char *in, uint32_t size) {
  uint32_t           total_written = 0;
  char               sector[512];
  struct NQFS_inode *fp = (struct NQFS_inode *)sector;
  if (!device->read_sector(device->device_no, inode_sector, sector)) {
    return 0;
  }
  char                       storage_sector[512];
  struct NQFS_storage_block *sb = (struct NQFS_storage_block *)storage_sector;

  if (fp->start_sector != 0) {
    uint32_t cur_size = fp->size;
    uint32_t read_sector = fp->start_sector;

    while (cur_size >= NQFS_STORAGE_BYTES_SECTOR) {
      if (!device->read_sector(device->device_no, read_sector,
                               storage_sector)) {
        return 0;
      }

      cur_size -= NQFS_STORAGE_BYTES_SECTOR;
      read_sector = sb->next_sector;
      if (read_sector == 0) {
        sb->next_sector = NQFS_get_next_free_block(device);
        read_sector = sb->next_sector;
        if (read_sector == 0) {
          return 0;
        }
      }
    }

    while (size > 0) {
      if (!device->read_sector(device->device_no, read_sector,
                               storage_sector)) {
        return 0;
      }

      uint32_t writeable = NQFS_STORAGE_BYTES_SECTOR - cur_size;
      if (size < writeable) {
        writeable = size;
      }

      // Copy data into struct
      memcpy(in + total_written, sb->data + cur_size, writeable);
      cur_size = 0;

      fp->size += writeable;
      total_written += writeable;

      if (!device->write_sector(device->device_no, inode_sector, sector)) {
        return 0;
      }

      // Write struct out to disk
      if (!device->write_sector(device->device_no, read_sector,
                                storage_sector)) {
        return 0;
      }
      size -= writeable;
      if (size > 0) {
        sb->next_sector = NQFS_get_next_free_block(device);
        // Write struct out to disk
        if (!device->write_sector(device->device_no, read_sector,
                                  storage_sector)) {
          return 0;
        }
        read_sector = sb->next_sector;
        if (read_sector == 0) {
          return 0;
        }
      }
    }
  } else {
    printf_nolock("File didn't point to a storage block\n");
    return 0;
  }

  return total_written;
}

void NQFS_DEBUG_list_files(struct block_dev *device) {
  char sector[512];
  for (int i = 0; i < FILEPTR_SECTORS; ++i) {
    if (!device->read_sector(device->device_no, i, sector)) {
      printf_nolock("Error reading sector %d\n", i);
      return;
    }
    struct NQFS_inode *fp = (struct NQFS_inode *)sector;
    for (int j = 0; j < FILEPTR_PER_SECTOR; ++j) {
      if (fp->allocd) {
        printf_nolock("Found file: %s\n", fp->filename);
      }
      fp++;
    }
  }
}
