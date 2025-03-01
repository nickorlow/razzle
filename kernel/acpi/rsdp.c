#include <stdint.h>

#include "../io/io.h"
#include "../memory/memory.h"
#include "rsdp.h"

struct rsdp_t {
  char     signature[8];
  uint8_t  checksum;
  char     oem_id[6];
  uint8_t  revision;
  uint32_t rsdt_address;
} __attribute__((packed));

struct xsdp_t {
  char     signature[8];
  uint8_t  checksum;
  char     oem_id[6];
  uint8_t  revision;
  uint32_t unused;
  uint32_t length;
  uint32_t xsdt_addr_hi;
  uint32_t xsdt_addr_lo;
  uint8_t  checksum_ext;
  uint8_t  unused_[3];
} __attribute__((packed));

struct acpi_header_t {
  char     signature[4];
  uint32_t length;
  uint8_t  revision;
  uint8_t  checksum;
  char     oem_id[6];
  char     oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_division;
} __attribute__((packed));

struct rsdt_t {
  struct acpi_header_t header;
  uint32_t             pointer[64];
};

uint8_t validate(struct rsdp_t *ptr) {
  char sig[] = "RSD PTR ";
  if (!memseq(ptr->signature, sig, 8))
    return 0;

  uint8_t *rsdp_bytes = (uint8_t *)ptr;
  uint8_t  sum = 0;
  for (uint8_t i = 0; i < 20; i++) {
    sum += rsdp_bytes[i];
  }

  if (sum == 0 && ptr->revision == 2) {
    printf_nolock("acpi: ACPI v2+ not yet supported :(\n");
    for (;;) {
    }
    // TODO: test with ACPI 2 System
    struct xsdp_t *r2_ptr = (struct xsdp_t *)ptr;

    uint8_t *xsdp_bytes = ((uint8_t *)r2_ptr) + 8;
    for (uint8_t i = 0; i < 16; i++) {
      sum += xsdp_bytes[i];
    }
  }

  return sum == 0;
}

char *find_table(struct rsdt_t *table, char *sig) {
  uint32_t entries = (table->header.length - sizeof table->header) / 4;
  for (uint32_t i = 0; i < entries; i++) {
    struct acpi_header_t *entry = (struct acpi_header_t *)table->pointer[i];
    if (memseq(entry->signature, sig, 4)) {
      printf_nolock("Found %s /  %s\n", entry->oem_id, entry->signature);
      printf_nolock("Signature: %s\n", entry->signature);
      printf_nolock("Length   : %d\n", entry->length);
      printf_nolock("Oem ID   : %s\n", entry->oem_id);
      printf_nolock("Checksum : %d\n", entry->checksum);
      return (void *)entry;
    }
  }

  printf_nolock("Not Found");
  return 0;
}

void find_rsdp() {
  struct rsdp_t *ptr;

  volatile uint8_t found = 0;
  for (uint32_t addr = 0x00000000; addr <= 0x000FFFFF; addr++) {
    ptr = (struct rsdp_t *)addr;

    if (validate(ptr)) {
      found = 1;
      break;
    }
  }

  if (!found) {
    uint8_t *ebda = (uint8_t *)(*((uint16_t *)0x40e) << 4);
    for (uint32_t i = 0; i <= 1024; i += 16) {
      ptr = (struct rsdp_t *)ebda;
      if (validate(ptr)) {
        found = 1;
        break;
      }
    }
  }

  printf_nolock("acpi: RSDP Found [OEM: %s, SIG:  %s]\n", ptr->oem_id,
                ptr->signature);
}
