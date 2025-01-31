#include <stdint.h>
#include "loader.h"
#include "casiowin.h"
#include <string.h>

/* Everything is loaded in RAM, so that's pretty easy. We just need to clear
   the BSS section and call the constructors. */
extern uint32_t ld_bbss, ld_ebss;
extern void (*ld_bctors)(void), (*ld_ectors)(void);
extern void (*ld_bdtors)(void), (*ld_edtors)(void);

extern int main(void);
extern void CASIOWIN_SetAPI(int API_version);

__attribute__((section(".text.entry")))
void start(void)
{
    /* Clear the BSS section */
    uint32_t *ptr = &ld_bbss;
    while(ptr < &ld_ebss)
        *ptr++ = 0;

    /* Call constructors */
    void (**ctor)(void) = &ld_bctors;
    while(ctor < &ld_ectors)
        (*ctor++)();

    /* Figure out the Math+/CG-100 API version based on OS version. */
    char const *version = (void *)0xa0020020;
    int API_version = 0;

    if(!memcmp(version, "01.00", 5))
        API_version = 0;
    else if(!memcmp(version, "02.00", 5))
        API_version = 1;

    CASIOWIN_SetAPI(API_version);

    int action = main();

    /* Make sure the MMU is disabled before we leave. The OS will crash in many
       spots if we keep it on. */
    MMU_SetEnabled(false);

    /* Call destructors */
    void (**dtor)(void) = &ld_bdtors;
    while(dtor < &ld_edtors)
        (*dtor++)();

    if(action == RETURN_USB_POPUP)
        CW_INTERNAL_USBPopup();
}
