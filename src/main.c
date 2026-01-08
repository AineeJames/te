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
  printf("Usage: %s path/to/game\n", prog_name);
}

static bool verify_game_path(const char *game_path) {
  if (!DirectoryExists(game_path)) {
    error("Game path should be a directory!");
    return false;
  }

  if (!FileExists(TextFormat("%s/main.lua", game_path))) {
    error("Expected a main.lua to exist in the game directory!");
    return false;
  }

  return true;
}

int main(int argc, char *argv[]) {
  const char *prog_name = argv[0];

  if (argc != 2) {
    error("An incorrect number of arguments was provided!");
    usage(prog_name);
    return EXIT_FAILURE;
  }

  const char *game_path = argv[1];
  if (!verify_game_path(game_path)) {
    return EXIT_FAILURE;
  }

  Engine *engine = engine_init(game_path);
  int exit_code = engine_run(engine);
  engine_free(engine);

  return exit_code;
}
