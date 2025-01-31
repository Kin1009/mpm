#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Types */
typedef unsigned int uint;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define PACKED(N) __attribute__((packed, aligned(N)))

/* PrintMini() with reasonable defaults and transparent bg. */
int PrintMini(int x, int y, char const *str, int fg);
/* Get the length of a PrintMini() string. */
int PrintMiniLength(char const *str);
/* PrintMini() with printf-formatting. */
int PrintfMiniColor(int x, int y, int fg, char const *fmt, ...);
/* Same in black. */
#define PrintfMini(x, y, ...) PrintfMiniColor(x, y, 0, __VA_ARGS__)

/* PrintMM() with reasonable defaults and transparent bg. */
int PrintMM(int x, int y, char const *str, int fg);
/* PrintMM with an outline. */
void PrintMMOutline(int x, int y, char const *str, int fg, int outline);
/* Get the length of a PrintMM() string. */
int PrintMMLength(char const *str);

/* PrintMiniMini() with reasonable defaults. */
void PrintMiniMini(int x, int y, char const *str, int color3bit);
/* PrintMiniMini() with printf-formatting. */
void PrintfMiniMini(int x, int y, int color3bit, char const *fmt, ...);

/* Length of a FONTCHARACTER path. */
int PathStrlen(u16 const *path);
/* Duplicate a path and add a prefix to it (can be NULL). Free returned pointer
   with CW_free(). */
u16 *PathDuplicate(u16 const *path, u16 const *prefix);
/* Convert path to ASCII in str, where str is a buffer of size len, truncating
   if needed to ensure that str is properly NUL-terminated. Returns str. */
char *PathToStr(char *str, int len, u16 const *path);
