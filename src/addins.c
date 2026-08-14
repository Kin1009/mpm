#include "addins.h"
#include "casiowin.h"
#include <string.h>

#define ADDIN_SLAB_SIZE 256
static struct AddIn slab_addins[ADDIN_SLAB_SIZE];

//============================================================================//

void AddIn_load_metadata(struct AddIn *addin)
{
    if(addin->metadata_loaded || addin->metadata_error)
        return;

    int fd = CW_BFile_Open(addin->path, CW_BFile_ReadOnly);
    if(fd < 0) {
        addin->metadata_error = fd;
        return;
    }

    char *buffer;
    int rc = CW_BFile_Block(fd, 0, (void **)&buffer);
    if(rc < 0) {
        addin->metadata_error = rc;
        CW_BFile_Close(fd);
        return;
    }

    memcpy(addin->name, buffer + 0x040, 16);
    memcpy(addin->internal, buffer + 0x060, 11);
    memcpy(addin->version, buffer + 0x130, 10);
    memcpy(addin->date, buffer + 0x13c, 14);

    for(int i = 0; i < ADDIN_ICON_BLOCKS * 2; i++) {
        int rc = CW_BFile_Block(fd, 0x1000 + i * 0x1000,
            (void **)&addin->icon.blocks[i]);
        if(rc < 0) {
            addin->metadata_error = rc;
            CW_BFile_Close(fd);
            return;
        }
    }

    CW_BFile_Close(fd);
    addin->metadata_loaded = true;
}

void AddIn_render_icon(struct AddIn const *addin, int x, int y, bool selected)
{
    int w = 92, h = 64;
    if(x < 0 || x + w > 384 || y < 0 || y + h > 216)
        return;

    u16 *VRAM = (u16 *)CW_GetVRAMAddress() + 384 * y + x;
    u16 *const *blocks = selected ? addin->icon.sel : addin->icon.uns;

    for(int row = 0; row < h; row++) {
        int i = row * w / 0x800;
        int offset = row * w % 0x800;
        int chunk = 0x800 - offset;
        if (chunk < w) {
            memcpy(VRAM, blocks[i] + offset, chunk * sizeof(u16));
            memcpy(VRAM + chunk, blocks[i + 1], (w - chunk) * sizeof(u16));
        } else {
            memcpy(VRAM, blocks[i] + offset, w * sizeof(u16));
        }
        VRAM += 384;
    }

    /* Make sure we have a terminator */
    char str[17];
    memcpy(str, addin->name, 16);
    str[16] = 0;

    int width = PrintMMLength(str);

    int cx = x + (w - width) / 2;
    if(selected)
        PrintMMOutline(cx, y+45, str, 0x0000, 0xbf5f);
    else
        PrintMM(cx, y+45, str, 0x8c51);
}


void AddInList_init(struct AddInList *list, uint capacity)
{
    uint bytes = capacity * sizeof *list->addins;
    list->addins = CW_malloc(bytes);
    if(list->addins) {
        memset(list->addins, 0, bytes);
        list->capacity = capacity;
    }
    else {
        list->capacity = 0;
    }
    list->size = 0;
}

void AddInList_clear(struct AddInList *list)
{
    for(uint i = 0; i < list->size; i++) {
        struct AddIn *addin = list->addins[i];
        CW_free(addin->path);
    }
    list->size = 0;
}

int AddInList_size(struct AddInList *list)
{
    return list->size;
}

struct AddIn *AddInList_get(struct AddInList *list, int i)
{
    return ((uint)i < list->size) ? list->addins[i] : NULL;
}

struct AddIn *AddInList_extend(struct AddInList *list)
{
    if(list->size >= list->capacity)
        return NULL;

    struct AddIn *addin = &slab_addins[list->size];
    memset(addin, 0, sizeof *addin);
    list->addins[list->size] = addin;
    list->size++;
    return addin;
}
