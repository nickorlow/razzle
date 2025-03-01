#include "./pci.h"
#include "../../supp/pci_supp.c"
#include "../io/io.h"
#include "../memory/memory.h"

#ifdef ARCH_I386
#include "../../arch/i386/asm/asm.h"
#endif

struct pci_device pci_devices_list[PCI_MAX_DEVICES];
uint32_t          pci_devices_count;

uint16_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t function,
                         uint8_t offset) {
  uint32_t address = ((bus << 16) | (slot << 11) | (function << 8) |
                      (offset & 0xFC) | (8 << 28));

  outl(0xCF8, address);
  return (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

enum PCI_PARSE_STATE {
  PPS_NUM_1,
  PPS_NUM_2,
  PPS_NUM_3,
  PPS_NUM_4,
  PPS_SP,
  PPS_NAME,
  PPS_NEXTLN,
  PPS_TAB
};

enum PCI_PARSE_SSTATE { PPSS_VENDOR, PPSS_DEVICE };

uint32_t ascii_byte_to_num(char c) {
  if (c >= '0' && c <= '9') {
    return 0x0 + (c - '0');
  } else if (c >= 'A' && c <= 'F') {
    return 0xA + (c - 'A');
  } else if (c >= 'a' && c <= 'f') {
    return 0xA + (c - 'a');
  } else {
    return -1;
  }
}

void pci_get_vendor_dev_name(uint16_t vendor_id, uint16_t device_id,
                             char *vendor_name, char *device_name) {
  int            len = ___build_supp_pci_pci_ids_len;
  unsigned char *buf = &___build_supp_pci_pci_ids[0];
  int            pps = PPS_NUM_1;
  int            ppss = PPSS_VENDOR;

  int j = 0;

  for (int i = 0; i < len; ++i) {
    // if we encounter newline, reset state
    if (buf[i] == '\n') {
      if (pps == PPS_NAME && ppss == PPSS_VENDOR) {
        ppss = PPSS_DEVICE;
        pps = PPS_TAB;
        vendor_name[j] = '\0';
        j = 0;
      }
      if (pps == PPS_NAME && ppss == PPSS_DEVICE) {
        device_name[j] = '\0';
        return;
      }
      pps = ppss == PPSS_VENDOR ? PPS_NUM_1 : PPS_TAB;
      continue;
    }

    // If we're waiting for next line, don't do anything else
    if (pps == PPS_NEXTLN)
      continue;

    // If we see a tab, we don't want to read device ids, so
    // just ignore
    if (buf[i] == '\t' && ppss == PPSS_VENDOR) {
      pps = PPS_NEXTLN;
      continue;
    }

    // If we see a tab, we don't want to read device ids, so
    // just ignore
    if (buf[i] == '\t' && ppss == PPSS_DEVICE) {
      if (pps == PPS_TAB) {
        pps = PPS_NUM_1;
      } else {
        pps = PPS_NEXTLN;
      }
      continue;
    }

    // If we see a comment, ignore
    if (buf[i] == '#' && pps == PPS_NUM_1) {
      pps = PPS_NEXTLN;
      continue;
    }

    uint32_t tablenum = ascii_byte_to_num(buf[i]);
    uint32_t vid = ppss == PPSS_VENDOR ? vendor_id : device_id;

    switch (pps) {
    case PPS_NUM_1:
      vid = (vid >> 12) & 0xF;
      if (vid == tablenum) {
        pps = PPS_NUM_2;
      } else {
        pps = PPS_NEXTLN;
      }
      break;
    case PPS_NUM_2:
      vid = (vid >> 8) & 0xF;
      if (vid == tablenum) {
        pps = PPS_NUM_3;
      } else {
        pps = PPS_NEXTLN;
      }
      break;
    case PPS_NUM_3:
      vid = (vid >> 4) & 0xF;
      if (vid == tablenum) {
        pps = PPS_NUM_4;
      } else {
        pps = PPS_NEXTLN;
      }
      break;
    case PPS_NUM_4:
      vid = vid & 0xF;
      if (vid == tablenum) {
        pps = PPS_SP;
      } else {
        pps = PPS_NEXTLN;
      }
      break;
    case PPS_SP:
      if (buf[i + 1] == ' ') {
        pps = PPS_SP;
      } else {
        pps = PPS_NAME;
      }
      break;
    case PPS_NAME:
      if (ppss == PPSS_VENDOR) {
        vendor_name[j] = buf[i];
        j++;
      } else {
        device_name[j] = buf[i];
        j++;
      }
      break;
    }
  }

  device_name[0] = '\0';
  vendor_name[0] = '\0';
}

struct pci_device pci_get_device(uint8_t bus, uint8_t slot) {
  struct pci_device found_dev;
  found_dev.vendor_id = pci_read_config(bus, slot, 0, 0);
  found_dev.device_id = pci_read_config(bus, slot, 0, 2);
  if (found_dev.vendor_id != PCI_NO_VENDOR) {
    pci_get_vendor_dev_name(found_dev.vendor_id, found_dev.device_id,
                            &found_dev.vendor_name[0],
                            &found_dev.device_name[0]);
    for (uint8_t i = 0; i < 6; i++) {
      found_dev.bar[i] =
          (uint32_t)(pci_read_config(bus, slot, 0, 0x12 + (i * 4)) << 16) |
          (pci_read_config(bus, slot, 0, 0x10 + (i * 4)));
    }
    uint16_t class_subclass = pci_read_config(bus, slot, 0, 4);
    found_dev.class_code = class_subclass & 0xFF;
    found_dev.subclass_code = (class_subclass >> 8) & 0xFF;
  }

  if (found_dev.vendor_name[0] == '\0' ||
      found_dev.vendor_id == PCI_NO_VENDOR) {
    strncpy("Unknown Vendor", &found_dev.vendor_name[0], 15);
    strncpy("Unknown Device", &found_dev.device_name[0], 15);
  }

  return found_dev;
}

void pci_enumerate_devices() {
  for (uint8_t bus = 0;; ++bus) {
    for (uint8_t slot = 0;; ++slot) {
      struct pci_device dev = pci_get_device(bus, slot);
      if (dev.vendor_id != PCI_NO_VENDOR) {
        pci_devices_list[pci_devices_count++] = dev;
        printf_nolock("pci: %d.%d: PCI device found bus %d slot %d vendor %x, "
                      "device %x (%d %d)\n",
                      bus, slot, bus, slot, dev.vendor_id, dev.device_id,
                      dev.class_code, dev.subclass_code);
        printf_nolock("pci: %d.%d: %s %s\n", bus, slot, dev.vendor_name,
                      dev.device_name);
      }
      if (slot == 255)
        break;
    }
    if (bus == 255)
      break;
  }
}
