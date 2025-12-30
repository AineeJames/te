/* slog - v1.0.0 - MIT License - https://github.com/aineejames/slog.h

  This library is aimed to easily add a logging system to a c project.

  # Basic example:
  ```c
  #define SLOG_IMPLEMENTATION
  #include "slog.h"

  int main(int argc, char *argv[]) {
    slog_set_handler(slog_color_handler); // Enable colored output (optional).

    slog(INFO, "Hello, world!");
    slog(FATAL, "Terminating program.");

    return EXIT_SUCCESS;
  }
  ```

  # Redefining loggin and fatal handlers:
  ```c
  #include <stdio.h>

  #define SLOG_IMPLEMENTATION
  #include "slog.h"

  void noop(void) { return; }

  void no_prefix_handler(slog_Level level, const char *fmt, va_list args) {
    vprintf(fmt, args);
    printf("\n");

    if (level == FATAL) {
      slog_fatal();
    }
  }

  int main(int argc, char *argv[]) {
    slog_set_fatal_handler(noop); // Configure FATAL logs to not exit.

    // Save off default handler
    slog_handler *save = slog_get_handler(); {
      slog_set_handler(no_prefix_handler);   // Use our no_prefix_handler

      slog(INFO, "No prefix to the left...");
    } slog_set_handler(save);                // Restore default log handler

    slog(INFO, "Prefix is back to normal...");

    slog(FATAL, "This wont cause the program to exit.");
    slog(INFO, "See, I told you so...");
  }
  ```

*/

#ifndef SLOG_H_
#define SLOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__GNUC__) || defined(__clang__)
#define SLOG_PRINTF_FORMAT(fmt_index, first_to_check)                          \
  __attribute__((format(printf, fmt_index, first_to_check)))
#elif defined(_MSC_VER)
#include <sal.h>
#define SLOG_PRINTF_FORMAT(fmt_index, first_to_check) _Printf_format_string_
#else
#define SLOG_PRINTF_FORMAT(fmt_index, first_to_check)
#endif

typedef enum {
  INFO,
  WARNING,
  ERROR,
  FATAL,
} slog_Level;

typedef void(slog_handler)(slog_Level level, const char *fmt, va_list args);
typedef void(slog_fatal_handler)(void);

void slog_set_handler(slog_handler *handler);
slog_handler *slog_get_handler(void);

void slog_set_fatal_handler(slog_fatal_handler *handler);
slog_fatal_handler *slog_get_fatal_handler(void);

void slog_default_handler(slog_Level level, const char *fmt, va_list args);
void slog_color_handler(slog_Level level, const char *fmt, va_list args);
void slog_default_fatal(void);

void slog(slog_Level level, const char *fmt, ...) SLOG_PRINTF_FORMAT(2, 3);

void slog_fatal(void);

#endif // SLOG_H_

#ifdef SLOG_IMPLEMENTATION

static slog_handler *slog__handler = &slog_default_handler;
static slog_fatal_handler *slog__fatal_handler = &slog_default_fatal;

void slog_set_handler(slog_handler *handler) { slog__handler = handler; }

slog_handler *slog_get_handler(void) { return slog__handler; }

void slog_set_fatal_handler(slog_fatal_handler *handler) {
  slog__fatal_handler = handler;
}

slog_fatal_handler *slog_get_fatal_handler(void) { return slog__fatal_handler; }

void slog_fatal(void) { slog__fatal_handler(); }

void slog_default_fatal(void) { exit(EXIT_FAILURE); }

void slog_default_handler(slog_Level level, const char *fmt, va_list args) {
  FILE *outfile = level == INFO || level == WARNING ? stdout : stderr;

  switch (level) {
  case INFO:
    fprintf(outfile, "[INFO] ");
    break;
  case WARNING:
    fprintf(outfile, "[WARNING] ");
    break;
  case ERROR:
    fprintf(outfile, "[ERROR] ");
    break;
  case FATAL:
    fprintf(outfile, "[FATAL] ");
    break;
  }

  vfprintf(outfile, fmt, args);
  fprintf(outfile, "\n");

  if (level == FATAL)
    slog_fatal();
}

void slog_color_handler(slog_Level level, const char *fmt, va_list args) {
  FILE *outfile = level == INFO || level == WARNING ? stdout : stderr;

  switch (level) {
  case INFO:
    fprintf(outfile, "\x1b[32m[INFO]\x1b[0m ");
    break;
  case WARNING:
    fprintf(outfile, "\x1b[33m[WARNING]\x1b[0m ");
    break;
  case ERROR:
    fprintf(outfile, "\x1b[31m[ERROR]\x1b[0m ");
    break;
  case FATAL:
    fprintf(outfile, "\x1b[35m[FATAL]\x1b[0m ");
    break;
  }

  vfprintf(outfile, fmt, args);
  fprintf(outfile, "\n");

  if (level == FATAL)
    slog_fatal();
}

void slog(slog_Level level, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  slog__handler(level, fmt, args);
  va_end(args);
}

#endif // SLOG_IMPLEMENTATION

/*
  Revision history:

    1.0.0 (2025-12-30) first release

*/

/*
  MIT License

  Copyright (c) 2025 Aiden Olsen

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
