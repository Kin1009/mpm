#define CASIOWIN_API_VERSIONS 2

.global _CW_malloc
_CW_malloc:
    bra     _CASIOWIN_call
    mov     #0, r0

.global _CW_free
_CW_free:
    bra     _CASIOWIN_call
    mov     #1, r0

.global _CW_realloc
_CW_realloc:
    bra     _CASIOWIN_call
    mov     #2, r0

.global _CW_Bdisp_PutDisp_DD
_CW_Bdisp_PutDisp_DD:
    bra     _CASIOWIN_call
    mov     #3, r0

.global _CW_Bdisp_AllClr_VRAM
_CW_Bdisp_AllClr_VRAM:
    bra     _CASIOWIN_call
    mov     #4, r0

.global _CW_GetVRAMAddress
_CW_GetVRAMAddress:
    bra     _CASIOWIN_call
    mov     #5, r0

.global _CW_GetVRAMBackgroundAddress
_CW_GetVRAMBackgroundAddress:
    bra     _CASIOWIN_call
    mov     #6, r0

.global _CW_PrintMini
_CW_PrintMini:
    bra     _CASIOWIN_call
    mov     #7, r0

.global _CW_PrintMiniMini
_CW_PrintMiniMini:
    bra     _CASIOWIN_call
    mov     #8, r0

.global _CW_MMPrint
_CW_MMPrint:
    bra     _CASIOWIN_call
    mov     #9, r0

.global _CW_MMPrintRef
_CW_MMPrintRef:
    bra     _CASIOWIN_call
    mov     #10, r0

.global _CW_DrawFrame
_CW_DrawFrame:
    bra     _CASIOWIN_call
    mov     #11, r0

.global _CW_MsgBoxPush
_CW_MsgBoxPush:
    bra     _CASIOWIN_call
    mov     #12, r0

.global _CW_GetKeyWait
_CW_GetKeyWait:
    bra     _CASIOWIN_call
    mov     #13, r0

.global _CW_GetKey
_CW_GetKey:
    bra     _CASIOWIN_call
    mov     #14, r0

.global _CW_BFile_Remove
_CW_BFile_Remove:
    bra     _CASIOWIN_call
    mov     #15, r0

.global _CW_BFile_Rename
_CW_BFile_Rename:
    bra     _CASIOWIN_call
    mov     #16, r0

.global _CW_BFile_Create
_CW_BFile_Create:
    bra     _CASIOWIN_call
    mov     #17, r0

.global _CW_BFile_Open
_CW_BFile_Open:
    bra     _CASIOWIN_call
    mov     #18, r0

.global _CW_BFile_Close
_CW_BFile_Close:
    bra     _CASIOWIN_call
    mov     #19, r0

.global _CW_BFile_Size
_CW_BFile_Size:
    bra     _CASIOWIN_call
    mov     #20, r0

.global _CW_BFile_Seek
_CW_BFile_Seek:
    bra     _CASIOWIN_call
    mov     #21, r0

.global _CW_BFile_GetPos
_CW_BFile_GetPos:
    bra     _CASIOWIN_call
    mov     #22, r0

.global _CW_BFile_Write
_CW_BFile_Write:
    bra     _CASIOWIN_call
    mov     #23, r0

.global _CW_BFile_Read
_CW_BFile_Read:
    bra     _CASIOWIN_call
    mov     #24, r0

.global _CW_BFile_Block
_CW_BFile_Block:
    bra     _CASIOWIN_call
    mov     #25, r0

.global _CW_BFile_FindFirst
_CW_BFile_FindFirst:
    bra     _CASIOWIN_call
    mov     #26, r0

.global _CW_BFile_FindNext
_CW_BFile_FindNext:
    bra     _CASIOWIN_call
    mov     #27, r0

.global _CW_BFile_FindClose
_CW_BFile_FindClose:
    bra     _CASIOWIN_call
    mov     #28, r0

.global _CW_BFile_NameToStr_ncpy
_CW_BFile_NameToStr_ncpy:
    bra     _CASIOWIN_call
    mov     #29, r0

.global _CW_BFile_StrToName_ncpy
_CW_BFile_StrToName_ncpy:
    bra     _CASIOWIN_call
    mov     #30, r0

.global _CW_OS_InnerWait_ms
_CW_OS_InnerWait_ms:
    bra     _CASIOWIN_call
    mov     #31, r0

.global _CW_RTC_GetTicks
_CW_RTC_GetTicks:
    bra     _CASIOWIN_call
    mov     #32, r0

.global _CW_RTC_GetDateTime
_CW_RTC_GetDateTime:
    bra     _CASIOWIN_call
    mov     #33, r0

.global _CW_RTC_SetDateTime
_CW_RTC_SetDateTime:
    bra     _CASIOWIN_call
    mov     #34, r0

.global _CW_Timer_Install
_CW_Timer_Install:
    bra     _CASIOWIN_call
    mov     #35, r0

.global _CW_Timer_Start
_CW_Timer_Start:
    bra     _CASIOWIN_call
    mov     #36, r0

.global _CW_Timer_Stop
_CW_Timer_Stop:
    bra     _CASIOWIN_call
    mov     #37, r0

.global _CW_Timer_Deinstall
_CW_Timer_Deinstall:
    bra     _CASIOWIN_call
    mov     #38, r0

.global _CW_USB_PollRX
_CW_USB_PollRX:
    bra     _CASIOWIN_call
    mov     #39, r0

.global _CW_USB_ClearRX
_CW_USB_ClearRX:
    bra     _CASIOWIN_call
    mov     #40, r0

.global _CW_USB_Open
_CW_USB_Open:
    bra     _CASIOWIN_call
    mov     #41, r0

.global _CW_USB_ForceClose
_CW_USB_ForceClose:
    bra     _CASIOWIN_call
    mov     #42, r0

.global _CW_USB_Read
_CW_USB_Read:
    bra     _CASIOWIN_call
    mov     #43, r0

.global _CW_USB_Write
_CW_USB_Write:
    bra     _CASIOWIN_call
    mov     #44, r0

.global _CW_BatteryIcon
_CW_BatteryIcon:
    bra     _CASIOWIN_call
    mov     #45, r0

.global _CW_UpdateBatteryState
_CW_UpdateBatteryState:
    bra     _CASIOWIN_call
    mov     #46, r0

.global _CW_INTERNAL_USBPopup
_CW_INTERNAL_USBPopup:
    bra     _CASIOWIN_call
    mov     #47, r0

_CASIOWIN_TABLE:
    .long 0x8025e0fc, 0x80366708  /* (#0) malloc */
    .long 0x8025dec8, 0x803664d4  /* (#1) free */
    .long 0x8025ec3c, 0x803672c8  /* (#2) realloc */
    .long 0x80049184, 0x8007a2d4  /* (#3) Bdisp_PutDisp_DD */
    .long 0x80049560, 0x8007a6b0  /* (#4) Bdisp_AllClr_VRAM */
    .long 0x8004579a, 0x8007569e  /* (#5) GetVRAMAddress */
    .long 0x80045794, 0x80075698  /* (#6) GetVRAMBackgroundAddress */
    .long 0x80047288, 0x8007718c  /* (#7) PrintMini */
    .long 0x800463c0, 0x800762c4  /* (#8) PrintMiniMini */
    .long 0x80118a88, 0x8018c328  /* (#9) MMPrint */
    .long 0x80118980, 0x8018c220  /* (#10) MMPrintRef */
    .long 0x80049ed4, 0x8007b024  /* (#11) DrawFrame */
    .long 0x0       , 0x8029a33c  /* (#12) MsgBoxPush */
    .long 0x8017be56, 0x802382fe  /* (#13) GetKeyWait */
    .long 0x8012ea7a, 0x801d2bba  /* (#14) GetKey */
    .long 0x802404d2, 0x80334212  /* (#15) BFile_Remove */
    .long 0x80240482, 0x803341c2  /* (#16) BFile_Rename */
    .long 0x802401b0, 0x80333ef0  /* (#17) BFile_Create */
    .long 0x8023fb90, 0x803338d0  /* (#18) BFile_Open */
    .long 0x8023fd0e, 0x80333a4e  /* (#19) BFile_Close */
    .long 0x8023fdc4, 0x80333b04  /* (#20) BFile_Size */
    .long 0x8023ff2c, 0x80333c6c  /* (#21) BFile_Seek */
    .long 0x8024003c, 0x80333d7c  /* (#22) BFile_GetPos */
    .long 0x8024025e, 0x80333f9e  /* (#23) BFile_Write */
    .long 0x80240082, 0x80333dc2  /* (#24) BFile_Read */
    .long 0x0       , 0x80333cf2  /* (#25) BFile_Block */
    .long 0x80240888, 0x803345c8  /* (#26) BFile_FindFirst */
    .long 0x80240b06, 0x80334846  /* (#27) BFile_FindNext */
    .long 0x80240c10, 0x80334950  /* (#28) BFile_FindClose */
    .long 0x80241b66, 0x803358a6  /* (#29) BFile_NameToStr_ncpy */
    .long 0x80241bac, 0x803358ec  /* (#30) BFile_StrToName_ncpy */
    .long 0x80216f4c, 0x8030a2dc  /* (#31) OS_InnerWait_ms */
    .long 0x8004babc, 0x8007cc0c  /* (#32) RTC_GetTicks */
    .long 0x801755b8, 0x80230f34  /* (#33) RTC_GetDateTime */
    .long 0x80175418, 0x80230d94  /* (#34) RTC_SetDateTime */
    .long 0x800b130c, 0x8010de28  /* (#35) Timer_Install */
    .long 0x800b1456, 0x8010df72  /* (#36) Timer_Start */
    .long 0x800b14b2, 0x8010dfce  /* (#37) Timer_Stop */
    .long 0x800b13d4, 0x8010def0  /* (#38) Timer_Deinstall */
    .long 0x0       , 0x8034da3e  /* (#39) USB_PollRX */
    .long 0x0       , 0x8034da66  /* (#40) USB_ClearRX */
    .long 0x0       , 0x8023edac  /* (#41) USB_Open */
    .long 0x0       , 0x8034ddd2  /* (#42) USB_ForceClose */
    .long 0x0       , 0x8034da12  /* (#43) USB_Read */
    .long 0x0       , 0x8034d9d2  /* (#44) USB_Write */
    .long 0x8023e31c, 0x8033207c  /* (#45) BatteryIcon */
    .long 0x8017cac0, 0x80238f68  /* (#46) UpdateBatteryState */
    .long 0x800aba08, 0x800e7988  /* (#47) INTERNAL_USBPopup */
