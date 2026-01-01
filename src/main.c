#define SLOG_IMPLEMENTATION
#include "slog.h"

#include "engine.h"
#include "raylib.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static void usage(const char *prog_name) {
  printf("Usage:\n"
         "    %s run  path/to/game\n"
         "    %s init new/game/path\n",
         prog_name, prog_name);
}

static bool verify_game_path(const char *game_path) {
  if (!DirectoryExists(game_path)) {
    slog(ERROR, "Game path should be a directory!");
    return false;
  }

  if (!FileExists(TextFormat("%s/main.lua", game_path))) {
    slog(ERROR, "Expected a main.lua to exist in the game directory!");
    return false;
  }

  return true;
}

static void log_handler(slog_Level level, const char *fmt, va_list args) {
  printf("[te::");
  switch (level) {
  case INFO:
    printf("info");
    break;
  case WARNING:
    printf("warn");
    break;
  case ERROR:
    printf("error");
    break;
  case FATAL:
    printf("fatal");
    break;
  }

  printf("] ");
  vprintf(fmt, args);
  printf("\n");

  if (level == FATAL) {
    slog_fatal();
  }
}

static bool init_new_game(const char *game_path) { return true; }

int main(int argc, char *argv[]) {
  const char *prog_name = argv[0];

  slog_set_handler(log_handler);

  if (argc != 3) {
    slog(ERROR, "An incorrect number of arguments was provided!");
    usage(prog_name);
    return EXIT_FAILURE;
  }

  const char *command = argv[1];
  const char *game_path = argv[2];

  if (TextIsEqual(command, "run")) {
    if (!verify_game_path(game_path)) {
      return EXIT_FAILURE;
    }

    Engine *engine = engine_init(game_path);
    int exit_code = engine_run(engine);
    engine_free(engine);

    return exit_code;

  } else if (TextIsEqual(command, "init")) {

    // TODO: Can't be bothered to write this code riht now. Come up w/ an
    // elegant solution later.
    if (system(TextFormat("[ ! -d \"%s\" ] && cp -r \"template\" \"%s\"",
                          game_path, game_path)) != 0) {
      slog(ERROR, "Failed to initialize new game!");
      return EXIT_FAILURE;
    };

    slog(INFO, "Successfully created new game at '%s'", game_path);
    return EXIT_SUCCESS;

  } else {
    slog(ERROR, "Unknown command '%s'", command);
    usage(prog_name);
    return EXIT_FAILURE;
  }
}
