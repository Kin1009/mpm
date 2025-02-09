#include "logo.h"
#include "build-cg/logo.inc"

void Logo_Render(u16 *vram, int x, int y, int width)
{
    u16 const *data = logo;
    vram += y * width + x;

    for(int dy = 0; dy < LOGO_HEIGHT; dy++) {
        for(int dx = 0; dx < LOGO_WIDTH; dx++)
            vram[dx] = *data++;
        vram += width;
    }
}
