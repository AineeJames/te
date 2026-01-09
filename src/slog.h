/* slog.h v1.0.0 - www.github.com/AineeJames/slog.h - MIT
 *
 * A simple single header logging library.
 *
 * Cheatsheet:
 *    debug(fmt, ...);
 *    info(fmt, ...);
 *    warning(fmt, ...);
 *    error(fmt, ...);
 *    fatal(fmt, ...);   // If using the default handler, this will call
 *                          exit(EXIT_FAILURE).
 *
 *    Slog_Handler *handler = slog_get_handler(); // Gets the current handler.
 *    slog_set_handler(Slog_Handler *handler);    // Sets the logging handler.
 *    slog_set_level(Slog_Level level);           // Sets the base logging
 *                                                   level.
 *
 * Available handlers:
 *    slog_default_handler // This is the default handler used by slog.h
 *    slog_color_handler   // Same as [slog_default_handler] but colorful.
 */

#ifndef SLOG_H
#define SLOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
#define SLOG_PRINTF_FORMAT(FormatIdx, FirstArg)                                \
  __attribute__((format(printf, FormatIdx, FirstArg)))
#else
#define SLOG_PRINTF_FORMAT(FormatIdx, FirstArg)
#endif

typedef enum {
  SLOG_DEBUG,
  SLOG_INFO,
  SLOG_WARNING,
  SLOG_ERROR,
  SLOG_FATAL,
} Slog_Level;

#define debug(fmt, ...)                                                        \
  slog(__FILE__, __LINE__, SLOG_DEBUG, (fmt), ##__VA_ARGS__)
#define info(fmt, ...) slog(__FILE__, __LINE__, SLOG_INFO, (fmt), ##__VA_ARGS__)
#define warning(fmt, ...)                                                      \
  slog(__FILE__, __LINE__, SLOG_WARNING, (fmt), ##__VA_ARGS__)
#define error(fmt, ...)                                                        \
  slog(__FILE__, __LINE__, SLOG_ERROR, (fmt), ##__VA_ARGS__)
#define fatal(fmt, ...)                                                        \
  slog(__FILE__, __LINE__, SLOG_FATAL, (fmt), ##__VA_ARGS__)

void slog(const char *file, int line, Slog_Level level, const char *fmt, ...)
    __attribute__((format(printf, 4, 5)));

typedef struct {
  void *ctx;
} Slog_Handler_Opt;

typedef struct {
  const char *file;
  int line;
} Slog_Src;

typedef struct {
  Slog_Src src;
  Slog_Level level;
  const char *fmt;
  va_list args;
  void *ctx;
} Slog_Record;

typedef void(Slog_Handler)(Slog_Record *record);

void slog_default_handler(Slog_Record *record);
void slog_color_handler(Slog_Record *record);

#define slog_set_handler(handler, ...)                                         \
  slog_set_handler_opt((handler), (Slog_Handler_Opt){__VA_ARGS__})

void slog_set_handler_opt(Slog_Handler *handler, Slog_Handler_Opt opt);
Slog_Handler *slog_get_handler(void);

void slog_set_level(Slog_Level level);

#ifdef SLOG_IMPLEMENTATION

static Slog_Handler *__slog_handler = &slog_default_handler;
static void *__slog_ctx = NULL;
static Slog_Level __slog_level = SLOG_DEBUG;

void slog_set_level(Slog_Level level) { __slog_level = level; }

void slog_default_handler(Slog_Record *record) {
  printf("%s:%d ", record->src.file, record->src.line);

  switch (record->level) {
  case SLOG_DEBUG:
    printf("[DEBUG] ");
    break;
  case SLOG_INFO:
    printf("[INFO] ");
    break;
  case SLOG_WARNING:
    printf("[WARNING] ");
    break;
  case SLOG_ERROR:
    printf("[ERROR] ");
    break;
  case SLOG_FATAL:
    printf("[FATAL] ");
    break;
  }

  vprintf(record->fmt, record->args);
  printf("\n");

  if (record->level == SLOG_FATAL)
    exit(EXIT_FAILURE);
}

void slog_color_handler(Slog_Record *record) {
  printf("\x1b[2m%s:%d [\x1b[0m", record->src.file, record->src.line);

  switch (record->level) {
  case SLOG_DEBUG:
    printf("\x1b[34mDEBUG\x1b[0m"); // Blue
    break;
  case SLOG_INFO:
    printf("\x1b[32mINF0\x1b[0m"); // Green
    break;
  case SLOG_WARNING:
    printf("\x1b[33mWARNING\x1b[0m"); // Yellow
    break;
  case SLOG_ERROR:
    printf("\x1b[31mERROR\x1b[0m"); // Red
    break;
  case SLOG_FATAL:
    printf("\x1b[35mFATAL\x1b[0m"); // Magenta
    break;
  }

  printf("\x1b[2m]\x1b[0m ");
  vprintf(record->fmt, record->args);
  printf("\n");
}

void slog(const char *file, int line, Slog_Level level, const char *fmt, ...) {
  if (level < __slog_level)
    return;

  va_list args;
  va_start(args, fmt);

  Slog_Record record = {
      .src = {.file = file, .line = line},
      .level = level,
      .fmt = fmt,
      .ctx = __slog_ctx,
  };

  va_copy(record.args, args);

  __slog_handler(&record);

  va_end(record.args);
  va_end(args);
}

void slog_set_handler_opt(Slog_Handler *handler, Slog_Handler_Opt opt) {
  __slog_handler = handler;

  if (opt.ctx)
    __slog_ctx = opt.ctx;
}

Slog_Handler *slog_get_handler(void) { return __slog_handler; }

#endif // SLOG_IMPLEMENTATION

#endif // SLOG_H

/* Revision history:
 *    v1.0.0 - Initial release of slog.h
 */
