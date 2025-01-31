#include "util.h"
#include "casiowin.h"
#include <stdio.h>

int PrintMini(int x, int y, char const *str, int fg)
{
    CW_PrintMini(&x, &y, str, 0x42, -1, 0, 0, fg, -1, 1, 0);
    return x;
}

int PrintMiniLength(char const *str)
{
    int x = 0, y = 0;
    CW_PrintMini(&x, &y, str, 0x42, -1, 0, 0, 0, -1, 1, 1);
    return x;
}

int PrintfMiniColor(int x, int y, int fg, char const *fmt, ...)
{
    char str[64];

    va_list args;
    va_start(args, fmt);
    vsnprintf(str, 64, fmt, args);
    va_end(args);

    return PrintMini(x, y, str, fg);
}

int PrintMM(int x, int y, char const *str, int fg)
{
    CW_MMPrintRef(&x, &y, str, 0x42, -1, 0, 0, fg, -1, 1, 0);
    return x;
}

void PrintMMOutline(int x, int y, char const *str, int fg, int outline)
{
    /* Outline effect by printing multiple times (didn't find the params) */
    CW_MMPrint(x,   y-1, str, 0x42, -1, 0, 0, outline, -1, 1, 0);
    CW_MMPrint(x-1, y,   str, 0x42, -1, 0, 0, outline, -1, 1, 0);
    CW_MMPrint(x+1, y,   str, 0x42, -1, 0, 0, outline, -1, 1, 0);
    CW_MMPrint(x,   y+1, str, 0x42, -1, 0, 0, outline, -1, 1, 0);
    CW_MMPrint(x,   y,   str, 0x42, -1, 0, 0, fg, -1, 1, 0);
}

int PrintMMLength(char const *str)
{
    int x = 0, y = 0;
    CW_MMPrintRef(&x, &y, str, 0x42, -1, 0, 0, 0, -1, 1, 1);
    return x;
}

void PrintMiniMini(int x, int y, char const *str, int color3bit)
{
    // +0x10: bold
    CW_PrintMiniMini(&x, &y, str, 0x40, color3bit, 0);
}

void PrintfMiniMini(int x, int y, int color, char const *fmt, ...)
{
    char str[64];

    va_list args;
    va_start(args, fmt);
    vsnprintf(str, 64, fmt, args);
    va_end(args);

    return PrintMiniMini(x, y, str, color);
}

int PathStrlen(u16 const *path)
{
    int i = 0;
    while(path[i] != 0x0000 && path[i] != 0xffff)
        i++;
    return i;
}

u16 *PathDuplicate(u16 const *path, u16 const *prefix)
{
    int len1 = prefix ? PathStrlen(prefix) : 0;
    int len2 = PathStrlen(path);
    u16 *copy = CW_malloc((len1 + len2 + 1) * sizeof *copy);
    if(copy) {
        for(int i = 0; i < len1; i++)
            copy[i] = prefix[i];
        for(int i = 0; i < len2 + 1; i++)
            copy[len1+i] = path[i];
    }
    return copy;
}

char *PathToStr(char *str, int len, u16 const *path)
{
    CW_BFile_NameToStr_ncpy(str, path, len - 1);
    /* Ensure there is always a terminator, even if we have to truncate */
    str[len - 1] = 0;
    /* NameToStr_ncpy() can put a 0xff-terminator: we don't want it */
    for(int i = 0; i < len; i++) {
        if(str[i] == (char)0xff)
            str[i] = 0;
    }
    return str;
}
