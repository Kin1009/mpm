/* Hardcoding build-cg is not elegant but I don't see where else we'd look. */
#include "build-cg/casiowin.inc"

.global _CASIOWIN_SetAPI
.global _CASIOWIN_GetAPI

/* Invoke a fixed function given by address.
   @r0: Internal function ID (from table in casiowin.inc) */
_CASIOWIN_call:
    mov     #CASIOWIN_API_VERSIONS, r1
    mulu.w  r0, r1
    mov.l   .API, r0
    mov.l   @r0, r2
    sts     macl, r1
    mov.l   .CASIOWIN_TABLE, r0
    add     r2, r1
    shll2   r1
    mov.l   @(r0, r1), r0

    /* Save all parameter registers in a buffer */
    mov.l   .buffer, r1
    sts     pr, r2
    mov.l   r2, @r1
    mov.l   r4, @(4,r1)
    mov.l   r5, @(8,r1)
    mov.l   r6, @(12,r1)
    mov.l   r7, @(16,r1)
    mov.l   r0, @(20,r1)

    /* Determine whether the MMU was running, and if so, stop it */
    mov.l   .MMU_TransitionOff, r2
    jsr     @r2
    nop
    mov.l   .buffer, r1
    mov.l   r0, @(24,r1)

    /* Restore the parameters and do the call */
    mov.l   @(20,r1), r0
    mov.l   @(4,r1), r4
    mov.l   @(8,r1), r5
    mov.l   @(12,r1), r6
    jsr     @r0
    mov.l   @(16,r1), r7

    /* If the MMU was enabled, restore it */
    mov.l   .buffer, r1
    mov.l   .MMU_SetEnabled, r2
    mov.l   @(24,r1), r4
    jsr     @r2
    mov.l   r0, @(24,r1)

    /* Forward the return value */
    mov.l   .buffer, r1
    mov.l   @r1, r2
    lds     r2, pr
    rts
    mov.l   @(24,r1), r0

_CASIOWIN_SetAPI:
    mov.l   .API, r0
    rts
    mov.l   r4, @r0

_CASIOWIN_GetAPI:
    mov.l   .API, r0
    rts
    mov.l   @r0, r0

.balign 4
.API:                   .long .CASIOWIN_API
.buffer:                .long _CWcall_buffer
.MMU_TransitionOff:     .long _MMU_TransitionOff
.MMU_SetEnabled:        .long _MMU_SetEnabled
.CASIOWIN_TABLE:        .long _CASIOWIN_TABLE

.data
.CASIOWIN_API:
    .long 0
_CWcall_buffer:
    .zero 28
