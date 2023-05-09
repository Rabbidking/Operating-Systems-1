
#include "vbr.h"
#include "kprintf.h"

void sweet(struct VBR* vbr){{
    kprintf("SWEET!\n");
    kprintf("Bytes per sector: %d\n", vbr->bytes_per_sector);
    kprintf("Sectors per cluster: %d\n", vbr->sectors_per_cluster);
    kprintf("Root cluster: %d\n", vbr->root_cluster);
    kprintf("OEM: %.8s\n", vbr->oem);
    kprintf("Label: %.11s\n", vbr->label );
    kprintf("SOUR\n");
}}
