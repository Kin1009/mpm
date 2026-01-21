#include "addins.h"
#include "casiowin.h"
#include <string.h>

/* Preloaded add-in array because these things are huge (28 kB each!) and we
   can only allocate up to 4 in OS heap, even less if some heap-intesive apps
   are running. */
#define ADDIN_SLAB_SIZE 8
static struct AddIn slab_addins[ADDIN_SLAB_SIZE];
static int slab_used = 0;

struct AddIn *AddIn_alloc(void)
{
    if(slab_used < ADDIN_SLAB_SIZE)
        return &slab_addins[slab_used++];
    else
        return CW_malloc(sizeof(struct AddIn));
}

void AddIn_free(struct AddIn *addin)
{
    if(addin >= &slab_addins[0] && addin < &slab_addins[ADDIN_SLAB_SIZE])
        {}
    else
        CW_free(addin);
}

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

    char buffer[0x14a];
    int rc = CW_BFile_Read(fd, buffer, 0x14a, 0);
    if(rc < 0) {
        addin->metadata_error = rc;
        CW_BFile_Close(fd);
        return;
    }

    memcpy(addin->name, buffer + 0x040, 16);
    memcpy(addin->internal, buffer + 0x060, 11);
    memcpy(addin->version, buffer + 0x130, 10);
    memcpy(addin->date, buffer + 0x13c, 14);

    int icon_sizes = sizeof addin->icon_uns + sizeof addin->icon_sel;
    rc = CW_BFile_Read(fd, addin->icon_uns, icon_sizes, 0x1000);
    if(rc < 0) {
        addin->metadata_error = rc;
        CW_BFile_Close(fd);
        return;
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
    u16 const *icon = selected ? addin->icon_sel : addin->icon_uns;

    for(int row = 0; row < h; row++) {
        for(int col = 0; col < w; col++)
            VRAM[col] = icon[col];
        VRAM += 384;
        icon += w;
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
}

void AddInList_clear(struct AddInList *list)
{
    for(uint i = 0; i < list->size; i++) {
        struct AddIn *addin = list->addins[i];
        CW_free(addin->path);
        AddIn_free(addin);
    }
    memset(list->addins, 0, list->capacity * sizeof *list->addins);
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

    struct AddIn *addin = AddIn_alloc();
    if(!addin)
        return NULL;

    memset(addin, 0, sizeof *addin);
    list->addins[list->size] = addin;
    list->size++;
    return addin;
}
