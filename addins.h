#include "util.h"

#define ADDIN_ICON_BLOCKS 3
struct AddIn {
    /* Full path to add-in, heap allocated. */
    u16 *path;
    /* File size, from the search results' file info. */
    u32 filesize;
    /* Whether we've loaded the metadata, or error code from failing. */
    bool metadata_loaded;
    int metadata_error;
    /* Usual g3a metadata (strings are not NUL-terminated!) */
    char name[16];
    char internal[11];
    char version[10];
    char date[14];
    /* Direct file block addresses for icons. */
    union {
        u16 *blocks[ADDIN_ICON_BLOCKS * 2];
        struct {
            u16 *uns[ADDIN_ICON_BLOCKS];
            u16 *sel[ADDIN_ICON_BLOCKS];
        };
    } icon;
};

/* Get the metadata by reading the g3a file. Sets addin->metadata_loaded as
   well as addin->metadata_error. No-op when called multiple times. */
void AddIn_load_metadata(struct AddIn *addin);

/* Render an add-in's icon. The icon must be fully within the screen, otherwise
   this is a no-op. Draws the name as well, but does not clip it. */
void AddIn_render_icon(struct AddIn const *addin, int x, int y, bool selected);

/* Heap-allocated list of heap-allocated add-in structures. */
struct AddInList {
    uint size;
    uint capacity;
    struct AddIn **addins;
};

/* Initialize a freshly-allocated AddInList structure. */
void AddInList_init(struct AddInList *list, uint capacity);
/* Free the storage and reset the structure. The list must have been previously
   initialized by AddInList_init(). Keeps the add-in array capacity. */
void AddInList_clear(struct AddInList *list);
/* Pretty explicit. */
int AddInList_size(struct AddInList *list);

/* Get add-in by index. Returns NULL if the index is out-of-bounds. Pointers
   are not invalidated when the size of the list changes. */
struct AddIn *AddInList_get(struct AddInList *list, int i);

/* Add one more add-in to the end of the list and returns a pointer to it.
   Returns NULL (and is a no-op) if any allocation fails. */
struct AddIn *AddInList_extend(struct AddInList *list);
