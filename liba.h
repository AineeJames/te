// liba.h

#ifndef LIBA_H
#define LIBA_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* --- Constants --- */
#ifndef LIBA_TEXT_FORMAT_BUFFER_SIZE
#define LIBA_TEXT_FORMAT_BUFFER_SIZE 1024
#endif
#ifndef LIBA_TEXT_FORMAT_NUM_BUFFERS
#define LIBA_TEXT_FORMAT_NUM_BUFFERS 5
#endif

#ifndef LIBA_DA_BASE_CAPACITY
#define LIBA_DA_BASE_CAPACITY 1
#endif
#ifndef LIBA_DA_CAPACITY_MULT
#define LIBA_DA_CAPACITY_MULT 2
#endif

/* --- Handlers ---- */
#ifndef LIBA_FATAL_HANDLER
#define LIBA_FATAL_HANDLER exit(EXIT_FAILURE);
#endif

/* --- Logging --- */
#define LIBA_LOG_LEVELS(X)                                                     \
  X(INFO, "\x1b[32m")                                                          \
  X(WARN, "\x1b[33m")                                                          \
  X(ERROR, "\x1b[31m")                                                         \
  X(FATAL, "\x1b[35m")

#define AS_ENUM(level, _) LIBA_##level,
typedef enum { LIBA_LOG_LEVELS(AS_ENUM) LIBA_LOG_COUNT } liba_LogLevel;
#undef AS_ENUM

#define AS_STR(level, _) #level,
const char *liba_LogLevel_str[LIBA_LOG_COUNT] = {LIBA_LOG_LEVELS(AS_STR)};
#undef AS_STR

#define AS_COLOR(_, color) color,
const char *liba_LogLevel_color[LIBA_LOG_COUNT] = {LIBA_LOG_LEVELS(AS_COLOR)};
#undef AS_COLOR

#define liba_log(level, fmt, ...) _liba_log(level, fmt, ##__VA_ARGS__)
void _liba_log(liba_LogLevel level, const char *fmt, ...);

#define LIBA_UNIMPLEMENTED()                                                   \
  liba_log(LIBA_FATAL, "%s:%d: %s is unimplemented!", __FILE__, __LINE__,      \
           __FUNCTION__);

#define liba_debug(x)                                                          \
  do {                                                                         \
    _Generic((x),                                                              \
        int: _liba_debug_int,                                                  \
        long: _liba_debug_long,                                                \
        long long: _liba_debug_llong,                                          \
        unsigned int: _liba_debug_uint,                                        \
        unsigned long: _liba_debug_ulong,                                      \
        unsigned long long: _liba_debug_ullong,                                \
        float: _liba_debug_float,                                              \
        double: _liba_debug_double,                                            \
        long double: _liba_debug_ldouble,                                      \
        char: _liba_debug_char,                                                \
        signed char: _liba_debug_schar,                                        \
        unsigned char: _liba_debug_uchar,                                      \
        _Bool: _liba_debug_bool,                                               \
        char *: _liba_debug_str,                                               \
        const char *: _liba_debug_str,                                         \
        default: _liba_debug_ptr)(#x, x, __FILE__, __LINE__);                  \
  } while (0)

static inline void _liba_debug_int(const char *name, int val, const char *file,
                                   int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %d", file, line, name, val);
}

static inline void _liba_debug_long(const char *name, long val,
                                    const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %ld", file, line, name, val);
}

static inline void _liba_debug_llong(const char *name, long long val,
                                     const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %lld", file, line, name, val);
}

static inline void _liba_debug_uint(const char *name, unsigned int val,
                                    const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %u", file, line, name, val);
}

static inline void _liba_debug_ulong(const char *name, unsigned long val,
                                     const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %lu", file, line, name, val);
}

static inline void _liba_debug_ullong(const char *name, unsigned long long val,
                                      const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %llu", file, line, name, val);
}

static inline void _liba_debug_float(const char *name, float val,
                                     const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %f", file, line, name, val);
}

static inline void _liba_debug_double(const char *name, double val,
                                      const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %lf", file, line, name, val);
}

static inline void _liba_debug_ldouble(const char *name, long double val,
                                       const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %Lf", file, line, name, val);
}

static inline void _liba_debug_char(const char *name, char val,
                                    const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = '%c'", file, line, name, val);
}

static inline void _liba_debug_schar(const char *name, signed char val,
                                     const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = '%c'", file, line, name, val);
}

static inline void _liba_debug_uchar(const char *name, unsigned char val,
                                     const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = '%c'", file, line, name, val);
}

static inline void _liba_debug_bool(const char *name, _Bool val,
                                    const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %s", file, line, name,
           val ? "true" : "false");
}

static inline void _liba_debug_str(const char *name, const char *val,
                                   const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = \"%s\"", file, line, name,
           val ? val : "NULL");
}

static inline void _liba_debug_ptr(const char *name, const void *val,
                                   const char *file, int line) {
  liba_log(LIBA_INFO, "%s:%d: %s = %p", file, line, name, val);
}

/* --- Dynamic Arrays --- */
#define liba_da(type)                                                          \
  struct {                                                                     \
    type *items;                                                               \
    size_t count;                                                              \
    size_t capacity;                                                           \
  }
#define liba_da_init() {0}

#define liba_da_append(da, ...)                                                \
  do {                                                                         \
    if ((da).count >= (da).capacity) {                                         \
      size_t new_cap = (da).capacity == 0                                      \
                           ? LIBA_DA_BASE_CAPACITY                             \
                           : (da).capacity * LIBA_DA_CAPACITY_MULT;            \
                                                                               \
      (da).items = (typeof((da).items))realloc((da).items,                     \
                                               new_cap * sizeof(*(da).items)); \
      if (!(da).items)                                                         \
        liba_log(LIBA_FATAL, "%s:%s: Failed to realloc dynamic array!",        \
                 __FILE__, __LINE__);                                          \
                                                                               \
      (da).capacity = new_cap;                                                 \
    }                                                                          \
                                                                               \
    (da).items[(da).count++] = (__VA_ARGS__);                                  \
  } while (0)

#define liba_da_foreach(da, it, idx)                                           \
  for (size_t idx = 0; idx < (da).count; idx++)                                \
    for (int _once = 1; _once; _once = 0)                                      \
      for (typeof(*(da).items) it = (da).items[idx]; _once; _once = 0)

#define liba_da_foreach_ptr(da, it, idx)                                       \
  for (size_t idx = 0; idx < (da).count; idx++)                                \
    for (int _once = 1; _once; _once = 0)                                      \
      for (typeof((da).items) it = &(da).items[idx]; _once; _once = 0)

#define liba_da_clear(da)                                                      \
  do {                                                                         \
    (da).count = 0;                                                            \
  } while (0)

#define liba_da_free(da)                                                       \
  do {                                                                         \
    free((da).items);                                                          \
    (da).items = NULL;                                                         \
    (da).count = 0;                                                            \
    (da).capacity = 0;                                                         \
  } while (0)

/* --- String Manipulation --- */
#define liba_str_format(fmt, ...)                                              \
  _liba_str_format(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
const char *_liba_str_format(const char *file, int line, const char *fmt, ...);

#define liba_str_contains(str, substr) (strstr(str, substr) != NULL)
#define liba_str_equals(a, b) (strcmp(a, b) == 0)
#define liba_str_startswith(str, prefix)                                       \
  (strncmp(str, prefix, strlen(prefix)) == 0)
#define liba_str_endswith(str, suffix)                                         \
  (strlen(str) >= strlen(suffix) &&                                            \
   strcmp(str + strlen(str) - strlen(suffix), suffix) == 0)

typedef liba_da(const char *) liba_da_str;
liba_da_str liba_str_split(const char *str, const char *delims);

/* --- Implementation --- */
#ifdef LIBA_IMPLEMENTATION

void _liba_log(liba_LogLevel level, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  bool is_piped = !isatty(fileno(stdout));

  if (is_piped) { // If piped, dont include ascii color sequences
    printf("[%s] ", liba_LogLevel_str[level]);
  } else {
    printf("%s[%s]\x1b[0m ", liba_LogLevel_color[level],
           liba_LogLevel_str[level]);
  }

  vprintf(fmt, args);
  va_end(args);

  printf("\n");

  if (level == LIBA_FATAL)
    LIBA_FATAL_HANDLER
}

const char *_liba_str_format(const char *file, int line, const char *fmt, ...) {
  static char buffers[LIBA_TEXT_FORMAT_NUM_BUFFERS]
                     [LIBA_TEXT_FORMAT_BUFFER_SIZE] = {0};
  static int index = 0;

  char *buffer = buffers[index];
  memset(buffer, 0, LIBA_TEXT_FORMAT_BUFFER_SIZE);

  if (fmt != NULL) {
    va_list args;
    va_start(args, fmt);
    int byte_count = vsnprintf(buffer, LIBA_TEXT_FORMAT_BUFFER_SIZE, fmt, args);
    va_end(args);

    if (byte_count >= LIBA_TEXT_FORMAT_BUFFER_SIZE) {
      char *trunc_buffer = buffers[index] + LIBA_TEXT_FORMAT_BUFFER_SIZE -
                           4; // Add four bytes for "...\0"
      snprintf(trunc_buffer, 4, "...");
      liba_log(LIBA_WARN,
               "%s:%d: String format exceeded the maximum length of %d "
               "defined by "
               "LIBA_TEXT_FORMAT_BUFFER_SIZE by %d bytes!",
               file, line, LIBA_TEXT_FORMAT_BUFFER_SIZE,
               byte_count - LIBA_TEXT_FORMAT_BUFFER_SIZE);
    }

    index += 1;
    if (index >= LIBA_TEXT_FORMAT_NUM_BUFFERS)
      index = 0;
  }

  return buffer;
}

liba_da_str liba_str_split(const char *str, const char *delims) {
  liba_da_str out = liba_da_init();
  if (!str)
    return out;

  char *copy = strdup(str);
  if (!copy)
    return out;

  char *token = strtok(copy, delims);
  while (token) {
    liba_da_append(out, strdup(token));
    token = strtok(NULL, delims);
  }

  free(copy);
  return out;
}

#endif // LIBA_IMPLEMENTATION

#ifndef LIBA_STRIP_PREFIX_GUARD_
#define LIBA_STRIP_PREFIX_GUARD_

#ifdef LIBA_STRIP_PREFIX

#define log liba_log
#define STRIP(level, _) enum { level = LIBA_##level };
LIBA_LOG_LEVELS(STRIP)
#undef STRIP
#define UNIMPLEMENTED LIBA_UNIMPLEMENTED

#define da liba_da
#define da_init liba_da_init
#define da_append liba_da_append
#define da_foreach liba_da_foreach
#define da_foreach_ptr liba_da_foreach_ptr
#define da_clear liba_da_clear
#define da_free liba_da_free

#define str_format liba_str_format
#define str_contains liba_str_contains
#define str_equals liba_str_equals
#define str_startswith liba_str_startswith
#define str_endswith liba_str_endswith
#define str_equals liba_str_equals
#define str_startswith liba_str_startswith
#define str_endswith liba_str_endswith
#define str_split liba_str_split

#endif // LIBA_STRIP_PREFIX

#endif // LIBA_STRIP_PREFIX_GUARD_

#endif // LIBA_H
