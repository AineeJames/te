#ifndef COLORS_H_
#define COLORS_H_

#define VGA_COLOR_LIST                                                         \
  X(BLACK)                                                                     \
  X(BLUE)                                                                      \
  X(GREEN)                                                                     \
  X(CYAN)                                                                      \
  X(RED)                                                                       \
  X(MAGENTA)                                                                   \
  X(BROWN)                                                                     \
  X(LIGHT_GRAY)                                                                \
  X(DARK_GRAY)                                                                 \
  X(LIGHT_BLUE)                                                                \
  X(LIGHT_GREEN)                                                               \
  X(LIGHT_CYAN)                                                                \
  X(LIGHT_RED)                                                                 \
  X(LIGHT_MAGENTA)                                                             \
  X(YELLOW)                                                                    \
  X(WHITE)

typedef enum {
#define X(name) VGA_##name,
  VGA_COLOR_LIST
#undef X
      VGA_COLOR_COUNT
} VGA_Color;

#endif
