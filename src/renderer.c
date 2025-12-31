#include "renderer.h"
#include "colors.h"
#include "globals.h"
#include "grid.h"
#include <assert.h>
#include <raylib.h>
#include <stdlib.h>

void render_frame(Engine *engine) {
  UnloadTexture(engine->renderer->grid_texture);
  engine->renderer->grid_texture = grid_render_texture(engine->grid);

  BeginDrawing();
  {
    ClearBackground(BLACK);
    BeginShaderMode(engine->renderer->grid_shader.shader);
    {
      SetShaderValueTexture(engine->renderer->grid_shader.shader,
                            engine->renderer->grid_shader.glyphAtlasTextureLoc,
                            engine->renderer->atlas.texture);
      SetShaderValueTexture(engine->renderer->grid_shader.shader,
                            engine->renderer->grid_shader.gridTextureLoc,
                            engine->renderer->grid_texture);

      DrawTexture(engine->renderer->dummy, 0, 0, WHITE);
    }
    EndShaderMode();
  }
  EndDrawing();
}

Renderer *renderer_init(Engine *engine) {
  Renderer *renderer = malloc(sizeof(Renderer));
  assert(renderer);

  renderer->atlas = (GlyphAtlas){
      .texture = LoadTexture("assets/images/Mx437_IBM_BIOS_16px.png"),
      .glyph_w = GLYPH_W,
      .glyph_h = GLYPH_H};

  renderer->grid_shader.shader = LoadShader(NULL, "assets/shaders/shader.glsl");

  // Cache shader locations
  renderer->grid_shader.glyphAtlasTextureLoc =
      GetShaderLocation(renderer->grid_shader.shader, "fontAtlasTexture");
  renderer->grid_shader.gridTextureLoc =
      GetShaderLocation(renderer->grid_shader.shader, "gridTexture");
  renderer->grid_shader.cellSizeLoc =
      GetShaderLocation(renderer->grid_shader.shader, "cellSize");
  renderer->grid_shader.gridSizeLoc =
      GetShaderLocation(renderer->grid_shader.shader, "gridSize");

  renderer->grid_texture = grid_render_texture(engine->grid);

  Image img = GenImageColor(GetScreenWidth(), GetScreenHeight(), WHITE);
  renderer->dummy = LoadTextureFromImage(img);
  UnloadImage(img);

  renderer->fg = VGA_WHITE;
  renderer->bg = VGA_BLACK;

  return renderer;
}

void renderer_free(Renderer *renderer) {
  UnloadTexture(renderer->atlas.texture);
  UnloadShader(renderer->grid_shader.shader);
  UnloadTexture(renderer->dummy);
  free(renderer);
}
