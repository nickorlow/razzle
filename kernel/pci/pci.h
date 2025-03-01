#pragma once

#include <stdint.h>

#define PCI_NO_VENDOR (0xFFFF)
#define PCI_MAX_NAMESIZE (128)
#define PCI_MAX_DEVICES (256)

struct pci_device {
  uint16_t device_id;
  uint16_t vendor_id;
  char     device_name[PCI_MAX_NAMESIZE];
  char     vendor_name[PCI_MAX_NAMESIZE];
  uint32_t bar[6];
  uint8_t  class_code;
  uint8_t  subclass_code;
};

extern struct pci_device pci_devices_list[PCI_MAX_DEVICES];
extern uint32_t          pci_devices_count;

void pci_enumerate_devices();
