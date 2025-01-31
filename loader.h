#pragma once
#include "util.h"

/* Return codes from main. */
#define RETURN_MAIN_MENU 0
#define RETURN_USB_POPUP 1

#define PTEH  ((u32 volatile *)0xff000000)
#define PTEL  ((u32 volatile *)0xff000004)
#define MMUCR ((u32 volatile *)0xff000010)

/* Address part of a UTLB entry. */
struct MMU_UTLB_Addr {
    u32 VPN    :22;
    u32 D      :1;
    u32 V      :1;
    u32 ASID   :8;
} PACKED(4);

/* Data part of a UTLB entry. */
struct MMU_UTLB_Data {
    uint        :3;
    uint PPN    :19;
    uint        :1;
    uint V      :1;
    uint SZ1    :1;
    uint PR     :2;
    uint SZ0    :1;
    uint C      :1;
    uint D      :1;
    uint SH     :1;
    uint WT     :1;
} PACKED(4);

/* Get the details of a UTLB entry (0 ≤ E < 64). */
void MMU_UTLB_Get(
    uint E, struct MMU_UTLB_Addr *addr, struct MMU_UTLB_Data *data);

/* Enable or disable the MMU. Is a no-op if MMU is already enabled/disabled. */
void MMU_SetEnabled(bool enabled);
/* Check if MMU is enabled. */
bool MMU_IsEnabled(void);
/* If the MMU is enabled, disable it and return true. Otherwise, return false.
   This can be used before a call to an MMU-sensitive function, restoring the
   state later with MMU_SetEnabled(). */
bool MMU_TransitionOff(void);

/* Map a page through MMU. The size in bytes must be 1 kB, 4 kB, 64 kB or 1 MB.
   If URC>=0, this specific entry is used, otherwise the current value of URC
   in MMUCR is used (which cannot work twice in a row in setup code). */
void MMU_Map(void *virt, void *phys, int size, int URC);

// TODO: Backup/check TLB when transitioning for syscalls.
